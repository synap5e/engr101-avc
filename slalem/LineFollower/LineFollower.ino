#include <QTRSensors.h>

#define NUM_SENSORS   6     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

#define pwm_left 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_right 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_left 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_right 13  //dir control for motor outputs 3 and 4 is on digital pin 13

#define motor_kill 8
  

// sensors 0 through 7 are connected to digital pins 3 through 10, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  A0, A1, A2, A3, A4, A5}
,
NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];


void setup()
{
  Serial.begin(9600);

  pinMode(pwm_left, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_right, OUTPUT);
  pinMode(dir_left, OUTPUT);
  pinMode(dir_right, OUTPUT);

  digitalWrite(dir_left, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_right, HIGH); //Set motor direction, 3 high, 4 low


  analogWrite(pwm_left, 0);	
  analogWrite(pwm_right, 0);

  Serial.println("Calibrating... ");

  delay(500);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  // print the calibration minimum values measured when emitters were on

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);

  Serial.println("done");
}

int lastError = 0;
#define KP 0.06
#define KD 1.500

#define M1 100 // left
#define M2 100 // right

void loop()
{
  
    if (digitalRead(motor_kill)){
    analogWrite(pwm_left, 0);	
    analogWrite(pwm_right, 0);
    return;
  } 
  // read calibrated sensor values and obtain a measure of the line position from 0 to 5000
  // To get raw sensor values, call:
  //  qtrrc.read(sensorValues); instead of unsigned int position = qtrrc.readLine(sensorValues);
  int pos = qtrrc.readLine(sensorValues, QTR_EMITTERS_ON, true);
  int error = (2 * (pos - 2500))/5;

 // Serial.println(error);

  int motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;

  int m1Speed = M1 - motorSpeed;
  int m2Speed = M2 + motorSpeed;



  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;

  Serial.print(error);
  Serial.print("\t");
  Serial.print(m1Speed);
  Serial.print("\t");
  Serial.println(m2Speed);
  
    analogWrite(pwm_left, m1Speed);//max(255, m1Speed));	
  analogWrite(pwm_right, m2Speed);//max(255, m2Speed));



  // analogWrite(pwm_left, m1Speed);	
  //  analogWrite(pwm_right, m2Speed);


//  delay(250);
}



