// basic code to move forward

#define FORWARD 0
#define LEFT 1
#define BACKWARD 2
#define RIGHT 3

int pwm_a = 3;  //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13

boolean going_forward = true;

void setup()
{
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low
  
  analogWrite(pwm_a, 255);                                                                                                                                           
  analogWrite(pwm_b, 255);                                                                                                                                           
                                                                                                                                                                     
}                                                                                                                                                                    
                                                                                                                                                                     
void loop() {
  if (going_forward) {
    tryGoForward();
  } else {
    tryGoBackward();
  }
//  delay(sleep);
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
  if (dir == FORWARD) {
    
  } else if (dir == BACKWARD) {
    
  } else {
    Serial.print("Invalid Direction in go()! Dir: ");
    Serial.println(dir);
  }
}

void turn(int dir) {
  if (dir == LEFT) {
    
  } else if (dir == RIGHT) {
    
  } else {
    Serial.print("Invalid Direction in turn()! Dir: ");
    Serial.println(dir);
  }
}

boolean canGo(int dir) {
  if (dir == LEFT) {
    
  } else if (dir == FORWARD) {
    
  } else if (dir == RIGHT) {
    
  } else if (dir == BACKWARD) {
    
  }
}
