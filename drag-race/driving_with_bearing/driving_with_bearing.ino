#include <ps2.h>

// orange
#define MDATA 6

// green
#define MCLK 7


#define pwm_a 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_b 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_a 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_b 13  //dir control for motor outputs 3 and 4 is on digital pin 13

#define motor_kill 8

#define max_power 150

/*
 * The distance from the mouse to the axis of rotation
 * The units are the units used by the mouse
 */
#define MOUSE_RADIUS 800.0

PS2 mouse(MCLK, MDATA);

long nextAdjust;

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
  Serial.println("PUFF 0.7.2 bootstrap initiated");


  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);

  pinMode(motor_kill, INPUT);


  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low

  analogWrite(pwm_a, 0);	
  analogWrite(pwm_b, 0);


  Serial.print("Waiting for kill switch... ");
  while(digitalRead(motor_kill));
  Serial.println("done");

  Serial.print("Bringing up mouse... ");

  mouse_init();

  Serial.println("done");

  analogWrite(pwm_a, max_power);	
  analogWrite(pwm_b, max_power);
  
  nextAdjust = millis() + 500;
}

long x = 0;
long y = 0;
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
long distance = 200000;

void loop(){
  recalc();

  if (digitalRead(motor_kill)){
    analogWrite(pwm_a, 0);	
    analogWrite(pwm_b, 0);
  } 
  else {

    // analogWrite(pwm_a, 0);	
  //  analogWrite(pwm_b, 0);
    analogWrite(pwm_a, max_power + max(min(bearing, 0)*4, -0.8) * max_power);
    analogWrite(pwm_b, max_power - min(max(bearing, 0)*4, 0.8) * max_power);
 
    //analogWrite(pwm_a, max_power - min(max(x/10, 0), 200)/4);	
    //analogWrite(pwm_b, max_power + max(min(x/10, 0), -200)/4);	
  }


  if (i++ % 50 == 0){

    //Serial.print("255 + min(max(pow(");
    //Serial.print(x);
    //Serial.print(", 1.25), 0), 150)");
    Serial.print(max_power + max(min(bearing, 0)*4, -0.8) * max_power);
    Serial.print(" | ");
    Serial.print(max_power - min(max(bearing, 0)*4, 0.8) * max_power);
    //Serial.print("255 + max(min(pow(");
    //Serial.print(x);
    //Serial.print(", 1.25), 0), -150)");
    Serial.print("\t\t"); 


    Serial.print(x);
    Serial.print(", ");
    Serial.print(y); 
    Serial.print(". Motor kill ");
    Serial.print(digitalRead(motor_kill)); 
    Serial.print(". Heading ");
    Serial.print(bearing);
    Serial.print("\t");
    Serial.println(nextAdjust);
  }
}



