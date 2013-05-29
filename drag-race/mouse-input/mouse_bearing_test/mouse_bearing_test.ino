#include <ps2.h>

#define MDATA 4
#define MCLK 5

/*
 * The distance from the mouse to the axis of rotation
 * The units are the units used by the mouse
 */
#define MOUSE_RADIUS 1835.0

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

int16_t x = 0;
int16_t y = 0;
float bearing = 0.0;

void recalc()
{
  char mstat;
  char mx;
  char my;

  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();

  /*
   * The mouse is mounted sideways, so x and y are switched.
   * Normally the first byte is x, and the second y, but due to
   * the switch the order is reversed
   */
  my = mouse.read();
  mx = mouse.read();
  

  bearing += mx / MOUSE_RADIUS;
  
  x += my * sin(bearing) ;
  y += -my * cos(bearing);
}

int i = 0;

void loop(){
  recalc();

  if (i++ % 50 == 0){
   Serial.print(x);
   Serial.print(", ");
   Serial.print(y); 
   Serial.print(". Heading ");
   Serial.println(bearing);
  }
}

