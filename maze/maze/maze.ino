#include <ps2.h>

#define MDATA 8
#define MCLK 9

#define FORWARD 0
#define LEFT 1
#define BACKWARD 2
#define RIGHT 3

#define pwm_a 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_b 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_a 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_b 13  //dir control for motor outputs 3 and 4 is on digital pin 13

#define IRFor 2
#define IRLef 4
#define IRBac 5
#define IRRig 6

#define motor_kill 2
/*
 * The distance from the mouse to the axis of rotation
 * The units are the units used by the mouse
 */
#define MOUSE_RADIUS 1835.0

// The values that need to be moved for one movement/turn
#define x_move 500
#define y_move 132

PS2 mouse(MCLK, MDATA);

boolean going_forward = true;   

long x = 0;
long y = 0;   

long x_to_travel = 0;
long bear_to_travel = 0;
float bearing = 0.0;

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

   pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
   pinMode(pwm_b, OUTPUT);
   pinMode(dir_a, OUTPUT);
   pinMode(dir_b, OUTPUT);
   
   pinMode(motor_kill, INPUT);

   
   pinMode(6, OUTPUT);
   pinMode(7, OUTPUT);

   //digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
   //digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low
  
   analogWrite(pwm_a, 255);	
   analogWrite(pwm_b, 255);

   mouse_init();
}

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
  
  x_to_travel -= abs(my * sin(bearing));
  bear_to_travel -= abs(mx/MOUSE_RADIUS);
}
  
int i = 0;

void loop(){
  recalc();

  if ((x_to_travel <= 0 && bear_to_travel <= 0) || digitalRead(motor_kill)){
    analogWrite(pwm_a, 0);	
    analogWrite(pwm_b, 0);
    
  
    if (going_forward) {
      tryGoForward();
    } else {
      tryGoBackward();
    }
  } else {
    /*
     analogWrite(pwm_a, 255 - min(max(pow(x, 1.25), 0), 150));	
     analogWrite(pwm_b, 255 + max(min(pow(x, 1.25), 0) * 1.2, -150));	

    
    if (x > 0){
      // too far right, turn left
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
    } else if (x < 0){
      // too far left, turn right 
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
    }
    */
  }
  

  if (i++ % 50 == 0){
   Serial.print(x);
   Serial.print(", ");
   Serial.print(y); 
   Serial.print(". Heading ");
   Serial.println(bearing);
   Serial.print(". Bear to travel: ");
   Serial.println(bear_to_travel); 
  }
}

void tryGoForward() {
  if (canGo(LEFT)) {
    turn(LEFT);
    go(FORWARD);
  } else if (canGo(FORWARD)) {
    go(FORWARD);
  } else if (canGo(RIGHT)) {
    turn(RIGHT);
    go(FORWARD);
  } else { // Dead end
    going_forward = false;
  }
}

void tryGoBackward() {
  if (canGo(RIGHT)) {
    turn(RIGHT);
    go(BACKWARD);
  } else if (canGo(BACKWARD)) {
    go(BACKWARD);
  } else if (canGo(LEFT)) {
    turn(LEFT);
    go(BACKWARD);
  } else { // Dead end
    going_forward = true;
  }
}

void go(int dir) {
  analogWrite(pwm_a, 255);	
  analogWrite(pwm_b, 255);
  if (dir == FORWARD) {
    digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
    digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low
    x_to_travel = x_move;
  } else if (dir == BACKWARD) {
    digitalWrite(dir_a, LOW);  //Set motor direction, 1 low, 2 high
    digitalWrite(dir_b, LOW); //Set motor direction, 3 high, 4 low
    
    x_to_travel = x_move;
  } else {
    Serial.print("Invalid Direction in go()! Dir: ");
    Serial.println(dir);
    analogWrite(pwm_a, 0);	
    analogWrite(pwm_b, 0);
  }
}

void turn(int dir) {
  if (dir == LEFT) {
    digitalWrite(dir_a, LOW);  //Set motor direction, 1 low, 2 high
    digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low
    
    bear_to_travel = y_move;
  } else if (dir == RIGHT) {
    digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
    digitalWrite(dir_b, LOW); //Set motor direction, 3 high, 4 low
    
    bear_to_travel = y_move;
  } else {
    Serial.print("Invalid Direction in turn()! Dir: ");
    Serial.println(dir);
  }
}

boolean canGo(int dir) {
  if (dir == LEFT) {
    return (digitalRead(IRLef) == HIGH);
  } else if (dir == FORWARD) {
    return (digitalRead(IRFor) == HIGH);
  } else if (dir == RIGHT) {
    return (digitalRead(IRRig) == HIGH);
  } else if (dir == BACKWARD) {
    return (digitalRead(IRBac) == HIGH);
  }
}
