#include <SoftwareSerial.h>

//#define debug

#define zb_txPin 5
#define zb_rxPin 4

SoftwareSerial xBeeSerial(zb_rxPin, zb_txPin); // RX, TX

struct xbee_packet {
  uint16_t length;
  uint8_t *data;
  uint8_t crc;
  uint8_t calculated_crc;
};

xbee_packet* pack = NULL;
int packetIndex;
unsigned int check_sum_total;

void packetReceived();

void setup() {
  Serial.begin(9600);
  xBeeSerial.begin(9600);
}




void loop() {
  uint8_t inByte;
  while ( xBeeSerial.available() > 0)
  {
    inByte = xBeeSerial.read();
    #ifdef debug
    Serial.println();
    Serial.print(inByte, HEX);
    #endif
    if (inByte == 0x7E && pack == NULL)
    {
      #ifdef debug
      Serial.println(" is header");
      #endif
      pack = (xbee_packet*)malloc(sizeof(xbee_packet));
      pack->data = NULL;
      packetIndex = -1;
      check_sum_total = 0;
    } 
    else if (pack == NULL){
      #ifdef debug
      Serial.print(".");
      #endif
      return;
    }
    else if (packetIndex == 0 || packetIndex == 1)
    {
      *((uint8_t*)pack + (1-packetIndex)) = inByte;
      #ifdef debug
      Serial.println(" is length");
      #endif
    } 
    else if ((packetIndex) >= 16 && packetIndex < pack->length+2)
    {
      #ifdef debug
      Serial.println(" is data");
      #endif
      
      check_sum_total += inByte;
      
      if (pack->data == NULL)
      {
        pack->data = (uint8_t*)malloc((pack->length - 14) * sizeof(uint8_t));
      } 
      pack->data[packetIndex-16] = inByte;
    }
    else if (packetIndex == pack->length+2)
    {
      #ifdef debug
      Serial.println(" is crc");
      #endif
      
      pack->crc = inByte;
      pack->calculated_crc = 0xFF - (check_sum_total & 0xFF);
            
      // The receiver of the packet only wants the length of the usefull data
      pack->length -= 14;
      packetReceived();
      free(pack->data);
      free(pack);
      pack = NULL;
    } 
    else {
      #ifdef debug
      Serial.println(" is junk"); 
      #endif
      
      check_sum_total += inByte;
    }
    
    packetIndex++;

  }

}

void packetReceived(){
  Serial.println("\r\n-------------------\r\nPacket recieved");
  Serial.print("length: ");
  Serial.println(pack->length);
  Serial.print("data: ");
  for (int i = 0; i < pack->length; i++){
    Serial.print(pack->data[i], HEX); 
    Serial.print(",");
  }
  Serial.println();
  Serial.print("crc: ");
  Serial.println(pack->crc, HEX); 
  Serial.print("calculated crc: ");
  Serial.println(pack->calculated_crc, HEX); 

}

