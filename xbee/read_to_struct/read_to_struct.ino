#include <SoftwareSerial.h>

//#define debug
#define debug_errors

#define MAX_PACKET_LENGTH 64

#define zb_txPin 5
#define zb_rxPin 4

SoftwareSerial xBeeSerial(zb_rxPin, zb_txPin); // RX, TX

struct xbee_packet {
  uint16_t length;
  uint8_t *data;
  uint8_t crc;
  uint8_t calculated_crc;
};

#define pwm_left 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_right 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_left 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_right 13  //dir control for motor outputs 3 and 4 is on digital pin 13

xbee_packet* pack = NULL;
int packetIndex;
unsigned int check_sum_total;

void packetReceived();

void setup() {
  Serial.begin(9600);
  xBeeSerial.begin(9600);
  

   pinMode(pwm_left, OUTPUT);  //Set control pins to be outputs
   pinMode(pwm_right, OUTPUT);
   pinMode(dir_left, OUTPUT);
   pinMode(dir_right, OUTPUT);
  
   analogWrite(pwm_left, 0);	
   analogWrite(pwm_right, 0);
}




void loop() {
  uint8_t inByte;
  while ( xBeeSerial.available() > 0)
  {
    inByte = xBeeSerial.read();
    #ifdef debug
    Serial.print(inByte, HEX);
    #endif
    if (inByte == 0x7E && pack == NULL)
    {
      #ifdef debug
      Serial.println(" is header");
      #endif
      pack = (xbee_packet*)malloc(sizeof(xbee_packet));
      if (pack == NULL){
           #ifdef debug_errors
           Serial.println("! DROPPING PACKET - MALLOC FOR PACKET FAILED");
           #endif
           free(pack);
           pack = NULL;
           return;
        }
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
    else if ((packetIndex) >= 14 && packetIndex < pack->length+2)
    {
      #ifdef debug
      Serial.println(" is data");
      #endif
      
      check_sum_total += inByte;
      
      if (pack->data == NULL)
      {
        if (pack->length > MAX_PACKET_LENGTH){
           #ifdef debug_errors
           Serial.println("! DROPPING PACKET - TOO LARGE");
           #endif
           free(pack);
           pack = NULL;
           return;
        }
        pack->data = (uint8_t*)malloc((pack->length - 12) * sizeof(uint8_t));
        if (pack->data == NULL){
           #ifdef debug_errors
           Serial.println("! DROPPING PACKET - MALLOC FOR DATA ARRAY FAILED");
           #endif
           free(pack);
           pack = NULL;
           return;
        }
      } 
      pack->data[packetIndex-14] = inByte;
    }
    else if (packetIndex == pack->length+2)
    {
      #ifdef debug
      Serial.println(" is crc");
      #endif
      
      pack->crc = inByte;
      pack->calculated_crc = 0xFF - (check_sum_total & 0xFF);
            
      // The receiver of the packet only wants the length of the usefull data
      pack->length -= 12;
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

