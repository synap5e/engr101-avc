#include <ps2.h>

#define MDATA 2
#define MCLK 3

#define RELATIVE 

PS2 mouse(MCLK, MDATA);

void mouse_init()
{
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}

void setup(){
   Serial.begin(9600);
  mouse_init();
}

uint8_t bitV(char value, char bitNumber)
{
  return (value & (1 << bitNumber)) != 0;
}

void loop(){
  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();
  
  Serial.print("Y overflow: ");
  Serial.print(bitV(mstat, 7));
  
  Serial.print(", X overflow: ");
  Serial.print(bitV(mstat, 6));
  
  Serial.print(", X sign bit: ");
  Serial.print(bitV(mstat, 5));
  
  Serial.print(", Y sign bit: ");
  Serial.print(bitV(mstat, 4));
  
  Serial.print(", Always 1: ");
  Serial.print(bitV(mstat, 3));
  
  Serial.print(", Middle button: ");
  Serial.print(bitV(mstat, 2));

  Serial.print(", Right button: ");
  Serial.print(bitV(mstat, 1));
  
  Serial.print(", Left button: ");
  Serial.print(bitV(mstat, 0));
  
  /* send the data back up */
  //Serial.print(mstat, BIN);
  Serial.print(", X=");
  Serial.print(mx, DEC);
  Serial.print(", Y=");
  Serial.print(my, DEC);
  Serial.println();
  
  delay(100);
}

