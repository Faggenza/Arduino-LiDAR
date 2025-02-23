#define FRAME_LENGTH        256
#define rxPin               16

#define RPS                 (float)0.05  //Per avere le rotazioni al secondo
#define RPM                 (int)3     //Per avere le rotazioni al minuto
#define ANGLE_RESOLUTION    (float)0.01 //Moltiplicatore per angolo misurato, in deg
#define DISTANCE_RESOLUTION (float)0.25 //Moltiplicatore per distanza misurata, in mm

uint8_t frame[FRAME_LENGTH];

void setup() {
  Serial.begin(230400);

  memset(frame, 0, 256); //imposta tutti i valori del frame a 0
  
  pinMode(rxPin, INPUT);
  
  Serial2.begin(230400, SERIAL_8N1, rxPin); //8bit, 1 bit stop, 0 bit parità
}

void loop() {
  memset(frame, 0, 256); //imposta il frame a 0 per una nuova acquisizione
  if( !getData() ){ //passa se l'acquisizione avviene correttamente
    if (!checkCommandWord()){ //passa se trova 173 al command word
      if(!crcCheck()){ //passa solo al crc corretto 
       //parsing pacchetto, calcolo angolo=startAngle+22.5*N/M
       // M=(PayloadL-5)/3          N è l'indice del sample nel pacchetto
       uint16_t payloadLength = frame[6];
       payloadLength <<= 8;
       payloadLength = payloadLength | frame[7]; //payload length
  
       uint16_t m = (payloadLength - 5) / 3; //numero di campioni nel paylaod
  
       //Da qui siamo nel paylaod
       //float rps = (uint8_t)frame[8]/(float)20.0f; //RPS o RPM per rotazioni al secondo o al minuto
       //ESP_LOGI("Info pacchetto", "%f r/s", rps);
       //9 10 due byte inutili
       //11 12 sono lo start angle di tutti i sample del frame
       uint16_t startAngle = frame[11];
       startAngle <<= 8;
       startAngle = startAngle | frame[12];
       float startAngleFloat = (float)startAngle / 100;
       //ESP_LOGI("Start Angle", "%d gradi", startAngleFloat);
  
       float rps = frame[8] * RPS;
       //itero tutti i campioni per ottenere informazioni su angolo e distanza
       //li iteriamo su N fino a M e abbiamo:
       // Angle = startAngle + 22.5/m * N
       // Distance = frame[14 + 3N, 15 + 3N] * 0.25 (mm)
  
       //vettori angolo, distanza
       float angleVect[m];
       float distanceVect[m];
       for (int n=0; n<=m; n++){
        angleVect[n] = startAngleFloat + 22.5f / m * n;
        uint16_t distance = frame[14 + 3*n];
        distance <<= 8;
        distance = distance | frame[15 + 3*n];
        distanceVect[n]= distance * DISTANCE_RESOLUTION;
       }
       Serial.print("A");
       for (int i =0; i<m; i++){
        Serial.print(",");
        Serial.print(angleVect[i]);
        Serial.print(",");
        Serial.print(distanceVect[i]);
       }
       Serial.println();
      }
     }else{
        //Pacchetto di tipo AE, errore sensore
        if ( !crcCheck() ){
        float rps = frame[8] * RPS;
        Serial.print("E");
        Serial.println(rps); //rotazioni al secondo
        }
     }
  }
}

//controlla CRC con somma cumulativa a 16 bit,
// 0 crc corretto, 1 errore
int crcCheck(){
  
  //frame length
  uint16_t l = frame[1];
  l <<= 8;
  l = l | frame[2];
  
  uint16_t crc = frame[l];
  crc <<= 8;
  crc = crc | frame[l+1]; 
  
  uint16_t somma = 0;
  
  for (int i=0; i<l ;i++){
    somma += frame[i];
  }
  if (crc == somma){
    //Serial.println("CRC Corretto");
    return 0;
  }else {
    //Serial.println("CRC Non Corretto");
    return 1;
  }
}

int getData(){
  while(Serial2.read() != (frame[0] = 170));
  int i = 1;
  uint16_t N = 256;
  while (i<N+2){
        while(!Serial2.available()); //bloccante se non ci sono dati pronti
        frame[i] = Serial2.read();
        if (i == 2) {
          N=frame[1]<<8 | frame[2];
        }
        if (i == 3 && frame[3]!=1 ) return 1;
        if (i == 4 && frame[4]!=97 ) return 1;
        i++;
  }
  return 0;
}

void printFrame(){
  Serial.print("Frame: ");
  for (int i=0; i<FRAME_LENGTH; i++){
    Serial.print(frame[i]);
    Serial.print(" ");
      }
   Serial.println();
}
int checkCommandWord(){
  if(frame[5] == 173){
    return 0; //AD frame di informazioni
  }else if (frame[5] == 174){
    return 1; //AE frame di errore
  }
}
