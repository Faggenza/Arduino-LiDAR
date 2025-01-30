#define RXD2 16
#define TXD2 17

int lidar_frame[140];
//da datasheet ci sono 240 misurazioni per giro
int distances[240]; //per 15 frame, 256 per 16 frame


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //inizializzazione vettore distanze
  for(int a = 0; a < 240; a++){ //256 se 16 frame
    distances[a] = 0;
  }
}

void loop() {
 // while(!Serial.available());
 //controllo il primo byte == AA. HEADER
 Serial.println(Serial1.read());
  if(Serial1.read() == 170){
    GetLidarData();
    for (int i =0; i<130; i++){
      Serial.print(lidar_frame[i]);
      Serial.print(" ");
    }
    Serial.println();
    //controllo che il Protocollo sia 01 e il frame type sia 61
    if(lidar_frame[2] == 1 && lidar_frame[3] == 97){
      //
      int circleSegment = ((lidar_frame[10]*256+lidar_frame[11])/2250);
      int points = (lidar_frame[6] - 5) / 3; //M
      if(points > 30){
        for(int p = 0; p < 15; p++){
          
          int distance = (lidar_frame[13 + p*6]*256 + lidar_frame[14 + p*6]) * 0.25;
          if(distance < 6000){
            distances[circleSegment*15+p] = distance;
          }
        }
      }
    }
  }
}

void GetLidarData(){
  //prendo tutto il pacchetto di 130 Byte
  for(int a = 0; a< 130; a++){
    while(!Serial1.available());
    lidar_frame[a] = Serial1.read();
  }
}
