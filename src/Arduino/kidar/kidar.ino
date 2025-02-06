rx_buffer= new uint8_t[448]
//da datasheet ci sono 240 misurazioni per giro
//int distances[240]; //per 15 frame, 256 per 16 frame
int parserIndex = 0;


void setup() {
  Serial.begin(115200);
//inizializzazione vettore distanze
//  for(int a = 0; a < 240; a++){ //256 se 16 frame
//    distances[a] = 0;
  }
}

void loop() {
 // while(!Serial.available());
 //controllo il primo byte == AA. HEADER
    //GetLidarData();
      uint8_t c = Serial.read();
      processByte(c);
      
//    controllo che il Protocollo sia 01 e il frame type sia 61
//    if(lidar_frame[2] == 1 && lidar_frame[3] == 97){
//      //
//      int circleSegment = ((lidar_frame[10]*256+lidar_frame[11])/2250);
//      int points = (lidar_frame[6] - 5) / 3; //M
//      if(points > 30){
//        for(int p = 0; p < 15; p++){
//          
//          int distance = (lidar_frame[13 + p*6]*256 + lidar_frame[14 + p*6]) * 0.25;
//          if(distance < 6000){
//            distances[circleSegment*15+p] = distance;
//          }
//        }
//      }
//    }
  }
}

void GetLidarData(){
  //prendo tutto il pacchetto di 130 Byte
  for(int a = 0; a< 130; a++){
    while(!Serial.available());
    lidar_frame[a] = Serial.read();
  }
}

void processByte(uint_8 c){
  if (parserIndex>448){
    parserIndex = 0;
  }
  rx_buffer[parser_idx++] = c;
    checksum += c;
  
    switch (parser_idx) {
    case 1:
      if (c != START_BYTE) {
        //result = ERROR_INVALID_VALUE;
        parser_idx = 0;
      } else
        checksum = c;
      break;
  
    case 2:
      break;
  
    case 3:
      packet_length = decodeUInt16(packet_header->packet_length);
      if (packet_length > max_packet_len_bytes_less_crc + sizeof(uint16_t))
        result = ERROR_INVALID_PACKET;
      break;
  
    case 4:
      if (c != PROTOCOL_VERSION)
        result = ERROR_INVALID_VALUE;
      break;
  
    case 5:
      if (c != PACKET_TYPE)
        result = ERROR_INVALID_VALUE;
      break;
  
    case 6:
      if (c != DATA_TYPE_RPM_AND_MEAS && c != DATA_TYPE_RPM_ONLY)
        result = ERROR_INVALID_VALUE;
      break;
  
    case 7: // data length MSB
      break;
  
    case 8: // data length LSB
      data_length = decodeUInt16(packet_header->data_length);
      if (data_length == 0 || data_length > max_data_len_bytes)
        result = ERROR_INVALID_PACKET;
      break;
  
    default:
      // Keep reading
      packet_length = decodeUInt16(packet_header->packet_length);
      if (parser_idx != packet_length + 2)
        break;
      }
}
