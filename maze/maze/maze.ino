#include <ps2.h>
#include <NewPing.h>


// TODO change these
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// orange
#define MDATA 6

// green
#define MCLK 7

#define FORWARD 0
#define LEFT 1
#define BACKWARD 2
#define RIGHT 3

#define pwm_left 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_right 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_left 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_right 13  //dir control for motor outputs 3 and 4 is on digital pin 13

#define IRFor 2
#define IRLefFro 4
#define IRLefBac 5
//#define IRBac 4
//#define IRRig 6

#define motor_kill 2
/*
 * The distance from the mouse to the axis of rotation
 * The units are the units used by the mouse
 */
#define MOUSE_RADIUS 1835.0

// The values that need to be moved for one movement/turn
#define drive_amount 500
#define turn_amount 0

#define motor_strength 128
#define turn_strength 165
#define weak_turn_strength 130

#define left_threshold 15

PS2 mouse(MCLK, MDATA);  
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
#define ping_speed 100 
unsigned long ping_timer;     // Holds the next ping time.

long x = 0;
long y = 0;   

long x_to_travel = 0;
long bear_to_travel = 0;
float bearing = 0.0;

boolean isReversed = false;

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
   Serial.println("Started");

   pinMode(pwm_left, OUTPUT);  //Set control pins to be outputs
   pinMode(pwm_right, OUTPUT);
   pinMode(dir_left, OUTPUT);
   pinMode(dir_right, OUTPUT);
   Serial.println("Outputs set");
   
   //pinMode(motor_kill, INPUT);
  
   analogWrite(pwm_left, 0);	
   analogWrite(pwm_right, 0);

   mouse_init();
   
   Serial.println("Mouse Started");
   
   Serial.print("Waiting for kill switch... ");
   while(digitalRead(motor_kill));
   Serial.println("done");

   Serial.println("Starting ping");
   ping_timer = millis();
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
  
  x_to_travel -= abs(my);
  bear_to_travel -= abs(mx/MOUSE_RADIUS);

  if (millis() >= ping_timer) {   // ping_speed milliseconds since last ping, do another ping.
    ping_timer += ping_speed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  
  //delay(500);
  //x_to_travel = 0;
  //bear_to_travel = 0;
}

unsigned int left_distance = -1;

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here! - no shit
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    left_distance = sonar.ping_result / US_ROUNDTRIP_CM; // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
  }
}

void loop(){
  calculateMovements();
}

void calculateMovements(){
  if (!isReversed){ //Driving forwards
    if (detectOpening(LEFT)){
      Serial.println("Going left");
      turn90ThenDrive(false);
    } else if (detectOpening(FORWARD)){
      Serial.println("Going forward");
      drive(drive_amount);
    } else if (detectOpening(RIGHT)){
      Serial.println("Going right");
      turn90ThenDrive(true);
    } else {
      Serial.println("Stopped");
      //isReversed = !isReversed;
    }
  } else { //Driving backwards
    if (detectOpening(RIGHT)){
      turn90ThenDrive(true);
    } else if (detectOpening(BACKWARD)){
      drive(drive_amount);
    } else if (detectOpening(LEFT)){
      turn90ThenDrive(false);
    } else {
      isReversed = !isReversed;
    }
  }
}

void turn90ThenDrive(boolean isRightSensor){
  isReversed = !isReversed;
  drive(drive_amount*2);
  isReversed = !isReversed;
  
  //Works out whether the robot should spin clockwise (equation is same as isRightSensor xor isReversed)
  boolean clockwise = (isRightSensor != isReversed);
  
  //Power the motors
  if (clockwise){
    setMotors(turn_strength, -weak_turn_strength);
  } else {
    setMotors(-weak_turn_strength, turn_strength);
  }
  
  //Wait until the robot has turned enough
  bear_to_travel = turn_amount;
  while (bear_to_travel > 0){
    recalc();
  }
  
  drive(drive_amount*10);
}

void drive(int distance){
  if (!isReversed){ //Driving forwards
    setMotors(motor_strength, motor_strength);
  } else { //Driving backward
    setMotors(-motor_strength, -motor_strength);
  }
  
  //Wait until either the robot has gone in the right direction or has run into a wall
  x_to_travel = distance;
  while (x_to_travel > 0 && ((detectOpening(FORWARD) && !isReversed) || (detectOpening(BACKWARD) && isReversed))){
    recalc();
  }
  
  //Stop the motors
  setMotors(0,0);
}

void setMotors(int left, int right){
  //Set the direction of each motor
  int leftValue = HIGH; //The left wheel is driving forward
  if (left < 0) leftValue = LOW; //The left wheel is driving backward
  digitalWrite(dir_left, leftValue);
  
  int rightValue = HIGH; //The left wheel is driving forward
  if (right < 0) rightValue = LOW; //The left wheel is driving backward
  digitalWrite(dir_right, rightValue);

  //Set the strength of the motors
  analogWrite(pwm_left, abs(left));	
  analogWrite(pwm_right, abs(right));
}

boolean detectOpening(int dir){
  if (dir == LEFT) { //return whether there is a left opening
    return (digitalRead(IRLefFro) == HIGH && digitalRead(IRLefBac) == HIGH && left_distance > left_threshold);
  } else if (dir == FORWARD) { //return whether there is a front opening
    return (digitalRead(IRFor) == HIGH);
  } else if (dir == RIGHT) { //return whether there is a right opening
    return (digitalRead(IRRig) == HIGH);
  } else if (dir == BACKWARD) { //return whether there is a back opening
    return true;
    return (digitalRead(IRBac) == HIGH);
  }
}

