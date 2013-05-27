// code for transmiting to XBee radio with particular addaress
// pins 4 and 5 are used for communication between Arduino and 
// XBee

#include <SoftwareSerial.h>

#define zb_txPin 5
#define zb_rxPin 4
#define N_ZB_FRAME 124    //  maximum size of the frame
int n_frame_to_ZB;        //  counter for the frame 
SoftwareSerial xBeeSerial(zb_rxPin, zb_txPin); // XBee constructor


char frame_to_ZB[N_ZB_FRAME]; // frame declaration

void setup()
{
  Serial.begin(9600);
  xBeeSerial.begin(9600);
  
}


void loop()
{
  byte inByte;
  while ( xBeeSerial.available() > 0) // 
  {
    inByte = -1;
    inByte = xBeeSerial.read();
    Serial.print("rcvd: ");
    Serial.println(inByte,HEX);
  }
  delay(2000);
  SendToZB();
  
}

// function to send data through XBee
void SendToZB(){
  
  int j;
  unsigned int check_sum_total = 0;
  unsigned int CRC = 0;
  
   // delimiter
  frame_to_ZB[0] = 0x7E;
  // length
  frame_to_ZB[1] = 0x00;
  frame_to_ZB[2] = 0x16; // length
  //API ID
  frame_to_ZB[3] = 0x10; // tramsmission frame indicator
  frame_to_ZB[4] = 0x00;
  // destination 64 bit address - broadcast
  frame_to_ZB[5] = 0x00;
  frame_to_ZB[6] = 0x13;
  frame_to_ZB[7] = 0xA2;
  frame_to_ZB[8] = 0x00;
  frame_to_ZB[9] = 0x40;
  frame_to_ZB[10] = 0x8B;
  frame_to_ZB[11] = 0x8B;
  frame_to_ZB[12] = 0x5B;
  // destination 16 bit address - broadcast
  frame_to_ZB[13] = 0xFF;
  frame_to_ZB[14] = 0xFE;
  // number of hops
  frame_to_ZB[15] = 0x00;
  //option
  frame_to_ZB[16] = 0x01;
  //data
  frame_to_ZB[17] = 48;
  frame_to_ZB[18] = 49;
  frame_to_ZB[19] = 50;
  frame_to_ZB[20] = 51;
  frame_to_ZB[21] = 52;
  frame_to_ZB[22] = 53;
  frame_to_ZB[23] = 54;
  frame_to_ZB[24] = 55;
  
  n_frame_to_ZB = 25;

  // add CRC to the packet
  for ( j = 3 ; j < n_frame_to_ZB ; j++){
    check_sum_total = check_sum_total + frame_to_ZB[j];
  }
  check_sum_total = check_sum_total & 0xFF;
  CRC = 0xFF - check_sum_total;
  //Serial.println(CRC);
  frame_to_ZB[n_frame_to_ZB] = CRC;
  n_frame_to_ZB = n_frame_to_ZB + 1;
  
  Serial.println("transmitting...");
  j = 0;
  while ( j < n_frame_to_ZB){
    xBeeSerial.write(frame_to_ZB[j]);
    Serial.print(frame_to_ZB[j],HEX);
    Serial.print(" ");
    j++;
  }
  Serial.println(" ");
  Serial.println("finished transmitting");

}
