// code for receiving data from XBee radio
// xBee is connected to pins 4 and 5


#include <SoftwareSerial.h>

#define zb_txPin 5
#define zb_rxPin 4

SoftwareSerial xBeeSerial(zb_rxPin, zb_txPin); // RX, TX

void setup()
{
  Serial.begin(9600);
  xBeeSerial.begin(9600);
}


void loop()
{
  byte inByte;
  while ( Serial.available() > 0)
  {
    xBeeSerial.write(Serial.read());
  }
  
}


