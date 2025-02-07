#define FRAME_LENGTH  256
#define rxPin 32
#define txPin 33

uint8_t frame[FRAME_LENGTH];

void setup() {
  Serial.begin(230400);
  memset(frame, 0, 256); //imposta tutti i valori del frame a 0
  
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  Serial2.begin(230400, SERIAL_8N1, rxPin, txPin); //8bit, 1 bit stop, 0 bit parità
}

void loop() {
  while(Serial2.available()) {
    if (Serial2.read() == (frame[0] = 170)){
      for (int i=1; i!=FRAME_LENGTH; i++){ //16 frame per pacchetto e 16 pacchetti per rotazione
        frame[i] = Serial2.read();
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
  
