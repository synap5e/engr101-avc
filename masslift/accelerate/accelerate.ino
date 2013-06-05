// basic code to move forward


int pwm_a = 3;  //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13


int mspeed = 0;

void setup()
{
  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low
  
  analogWrite(pwm_a, 0);	
  analogWrite(pwm_b, 0);
  
}

void loop()
{
  
  analogWrite(pwm_a, mspeed);	
  analogWrite(pwm_b, mspeed);
  
  mspeed = max(mspeed+1, 255);
  
  delay(20);
    
}

