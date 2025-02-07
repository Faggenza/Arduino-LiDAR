#define FRAME_LENGTH  256
#define rxPin 16
#define txPin 17

uint8_t frame[FRAME_LENGTH];

void setup() {
  Serial.begin(230400);
  memset(frame, 0, 256); //imposta tutti i valori del frame a 0
  
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  Serial2.begin(230400, SERIAL_8N1, rxPin, txPin); //8bit, 1 bit stop, 0 bit parità
}

void loop() {
  getData();
//  Serial.print("Frame: ");
//  for (int i=0; i<FRAME_LENGTH; i++){
//    Serial.print(frame[i]);
//    Serial.print(" ");
//      }
//   Serial.println();
   if ( crcCheck() ){
    if( checkCommandWord() ){
      //Frame con info e crc corretto
      //parsing pacchetto, calcolo angolo=startAngle+22.5*N/M
      // M=(PayloadL-5)/3          N è l'indice del sample nel pacchetto
    }
   }
}

//controlla CRC con somma cumulativa a 16 bit,
// 0 crc corretto, 1 errore
int crcCheck(){
  //frame length
  uint8_t l = frame[2];
  uint16_t somma = 0;
  for (int i=0; i<l ;i++){
    somma += frame[i];
  }
  uint16_t crc = frame[l];
  crc <<= 8;
  crc = crc | frame[l+1];
  if (crc == somma){
    return 0;
  }else return 1;
}
void getData(){
  while(Serial2.available()) {
    if (Serial2.read() == (frame[0] = 170)){
      for (int i=1; i!=FRAME_LENGTH; i++){ //16 frame per pacchetto e 16 pacchetti per rotazione
        frame[i] = Serial2.read();
      }
    }
  }  
}

int checkCommandWord(){
  if(frame[5] == 173){
    return 0; //AD frame di informazioni
  }else if (frame[5] == 174){
    return 1; //AE frame di errore
  }
}
