#include <RunningMedian.h>
#include <QTRSensors.h>

#define NUM_SENSORS   6     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2

#define pwm_left 3  //PWM control for motor outputs 1 and 2 is on digital pin 3
#define pwm_right 11  //PWM control for motor outputs 3 and 4 is on digital pin 11
#define dir_left 12  //dir control for motor outputs 1 and 2 is on digital pin 12
#define dir_right 13  //dir control for motor outputs 3 and 4 is on digital pin 13

#define motor_kill 8

#define M1 255 // left
#define M2 255 // right


// sensors 0 through 7 are connected to digital pins 3 through 10, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  A0, A1, A2, A3, A4, A5}
,
NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];

RunningMedian samples = RunningMedian(9);

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

  Serial.print("Calibrating");

  delay(500);
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    if (i % 40 == 0)
      Serial.print(".");
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  Serial.println();

  digitalWrite(dir_left, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_right, HIGH); //Set motor direction, 3 high, 4 low

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

  for (int i = 0; i < 10; i++){
    unsigned int minVal = -1;
    for (unsigned char i = 0; i < NUM_SENSORS; i++)
    {
      minVal = min(sensorValues[i], minVal);
    }
    samples.add(minVal); 
  }

  analogWrite(pwm_left, M1);	
  analogWrite(pwm_right, M2);
}

long last = millis();
#define threshhold 200
bool headingLeft = true;

void loop()
{

  if (digitalRead(motor_kill))
  {
    analogWrite(pwm_left, 0);	
    analogWrite(pwm_right, 0);
    return;
  }

  int pos = qtrrc.readLine(sensorValues, QTR_EMITTERS_ON, true);
  // starts at max value
  unsigned int val = -1;
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    val = min(sensorValues[i], val);
  }



  //unsigned int val = samples.getMedian();
  if (val < threshhold)
  {
    Serial.println("line");
    Serial.println(pos);
    analogWrite(pwm_left, 0);	
    analogWrite(pwm_right, 0);
    delay(1000);
    if (pos < 2500)
    {
      // turn left
      analogWrite(pwm_left, 0);	
      analogWrite(pwm_right, M2);

    } 
    else {
      analogWrite(pwm_left, M1);	
      analogWrite(pwm_right, 0);

    }
    delay(400);
    analogWrite(pwm_left, 0);	
    analogWrite(pwm_right, 0);

    delay(2000);
    
          analogWrite(pwm_left, M1);	
      analogWrite(pwm_right, M2);
      delay(500);
  } 


  if (millis() > last+500){
    last = millis();
    Serial.println(val);
  }

  /*   for (unsigned char i = 0; i < NUM_SENSORS; i++)
   {
   Serial.print(sensorValues[i]);
   Serial.print('\t');
   }
   */


}












