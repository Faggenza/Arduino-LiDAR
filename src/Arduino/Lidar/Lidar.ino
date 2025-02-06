#define FRAME_LENGTH  256

uint8_t frame[FRAME_LENGTH];

void setup() {
  Serial.begin(230400, SERIAL_8N1);
  memset(frame, 0, 256); //imposta tutti i valori del frame a 0
  
  //lidar = new CustomSoftwareSerial(rxPin, txPin);
  //lidar->begin(230400, CSERIAL_8N1);
}

void loop() {
  while(Serial.available()) {
    if (Serial.read() == (frame[0] = 170)){
      for (int i=1; i!=FRAME_LENGTH; i++){ //16 frame per pacchetto e 16 pacchetti per rotazione
        frame[i] = Serial.read();
      }
      Serial.print("Frame: ");
      for (int i=0; i<FRAME_LENGTH; i++){
        Serial.print(frame[i]);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
  
