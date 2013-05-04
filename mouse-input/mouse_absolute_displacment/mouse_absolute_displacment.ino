#include <ps2.h>

#define MDATA 2
#define MCLK 3

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

int xOffset = 0;
int yOffset = 0;

void recalc()
{
  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();
  
  xOffset += mx;
  yOffset += my;
}

int i = 0;

void loop(){
  recalc();
  if (i++ % 50 == 0){
   Serial.print(xOffset);
   Serial.print(", ");
   Serial.println(yOffset); 
  }
}

