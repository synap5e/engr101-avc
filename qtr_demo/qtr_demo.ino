#include <QTRSensors.h>

// This example is designed for use with eight QTR-1RC sensors or the eight sensors of a
// QTR-8RC module.  These reflectance sensors should be connected to digital inputs 3 to 10.
// The emitter control pin can optionally be connected to digital pin 2, or you can leave
// it disconnected and change the EMITTER_PIN #define below from 2 to QTR_NO_EMITTER_PIN.

// The setup phase of this example calibrates the sensor for ten seconds and turns on
// the pin 13 LED while calibration is going on.  During this phase, you should expose each
// reflectance sensor ot the lightest and darkest readings they will encounter.  For
// example, if you are making a line follower, you should slide the sensors across the
// line during the calibration phase so that each sensor can get a reading of how dark the
// line is and how light the ground is.  Improper calibration will result in poor readings.
// If you want to skip the calibration phase, you can get the raw sensor readings
// (pulse times from 0 to 2500 us) by calling qtra.read(sensorValues) instead of
// qtra.readLine(sensorValues).

// The main loop of the example reads the calibrated sensor values and uses them to
// estimate the position of a line.  You can test this by taping a piece of 3/4" black
// electrical tape to a piece of white paper and sliding the sensor across it.  It
// prints the sensor values to the serial monitor as numbers from 0 (maximum reflectance) 
// to 9 (minimum reflectance) followed by the estimated location of the line as a number
// from 0 to 5000.  1000 means the line is directly under sensor 1, 2000 means directly
// under sensor 2, etc.  0 means the line is directly under sensor 0 or was last seen by
// sensor 0 before being lost.  5000 means the line is directly under sensor 5 or was
// last seen by sensor 5 before being lost.


#define NUM_SENSORS   8     // number of sensors used
#define TIMEOUT	 2500  // waits for 2500 us for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2
#define debug

#define FAST_SPEED 120
#define SLOW_SPEED 80
#define SUPERSLOW_SPEED 60
#define THRESHOLD 1000

// sensors 0 through 7 are connected to digital pins 3 through 10, respectively
QTRSensorsRC qtrrc((unsigned char[]) {
  A1, A2, A3, A4, A5, 8}
,
NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];

int pwm_a = 3;  //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13

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

  delay(500);
  int i;
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on LED to indicate we are in calibration mode
  for (i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();	 // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  digitalWrite(13, LOW);     // turn off LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtrrc.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  //  delay(1000);
}

bool lastTurnedLeft = false;
bool lineLost = false;

void loop()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000, where 0 means directly under sensor 0 or the line was lost
  // past sensor 0, 1000 means directly under sensor 1, 200 means directly under sensor 2, etc.
  // Note: the values returned will be incorrect if the sensors have not been properly
  // calibrated during the calibration phase.  To get raw sensor values, call:
  //  qtra.read(sensorValues);
  unsigned int positions = qtrrc.readLine(sensorValues);

  //qtrrc.read(sensorValues);

  // print the sensor values as numbers from 0 to 9, where 0 means maximum reflectance and
  // 9 means minimum reflectance, followed by the line position
  unsigned char i;

  int threshold = 4;

  /*print out debug values for the sensor.
   *
   */
#ifdef debug

  Serial.print(", ");
  Serial.print(sensorValues[0]);
  Serial.print(", ");
  Serial.print(sensorValues[1]);
  Serial.print(", ");
  Serial.print(sensorValues[2]);
  Serial.print(", ");
  Serial.print(sensorValues[3]);
  Serial.print(", ");
  Serial.print(sensorValues[4]);
  Serial.print(", ");
  Serial.print(sensorValues[5]);
      
#endif

  long ave_left = (sensorValues[0] + sensorValues[1])/2;
  long ave_mid = (sensorValues[2] + sensorValues[3])/2;
  long ave_right = (sensorValues[4]+ sensorValues[5])/2;
  long ave_total = (ave_right + ave_mid + ave_left)/3;

  long mini = min(min(ave_left, ave_mid),ave_right); //determining the lowest average sensor values
  Serial.println(mini);

  if (mini < THRESHOLD){
    Serial.println("Have a line");
    if (lineLost == true){
       if (lastTurnedLeft == true){
         analogWrite(pwm_a, SLOW_SPEED);	
         analogWrite(pwm_b, FAST_SPEED);
         lineLost = false;
        }
        else if(lastTurnedLeft == false){
          analogWrite(pwm_a, FAST_SPEED);	 
          analogWrite(pwm_b, SLOW_SPEED);
        }
       lineLost = false;
       delay(2000);
      }
    else if (mini == ave_left){ 
      Serial.print("Go left");
      if (sensorValues[0] < sensorValues[1]){
      analogWrite(pwm_a, SUPERSLOW_SPEED);	 
      analogWrite(pwm_b, FAST_SPEED);}
      else{
      analogWrite(pwm_a, SLOW_SPEED);	 
      analogWrite(pwm_b, FAST_SPEED);} 
      lastTurnedLeft = true;
    }
    else if (mini == ave_mid){
      Serial.print("Go forward");
      analogWrite(pwm_a, FAST_SPEED);	
      analogWrite(pwm_b, FAST_SPEED);

    }

    else{
      Serial.print("Go right");
      if (sensorValues[5] < sensorValues[4]){
      analogWrite(pwm_a, FAST_SPEED);	 
      analogWrite(pwm_b, SUPERSLOW_SPEED);}
      else{
      analogWrite(pwm_a, FAST_SPEED);	 
      analogWrite(pwm_b, SLOW_SPEED);} 
      lastTurnedLeft = false;
    }

  } else {
    lineLost = true;
    
    Serial.println("Line lost");
    if(lastTurnedLeft){
      Serial.print("lost line - Go right");
      analogWrite(pwm_a, FAST_SPEED);	 
      analogWrite(pwm_b, SLOW_SPEED);


    }
    else {
      Serial.print("lost line - Go left");
      analogWrite(pwm_a, SLOW_SPEED);	
      analogWrite(pwm_b, FAST_SPEED);


    }
  }
  
  digitalWrite(dir_a, HIGH);  //Set motor direction, 1 low, 2 high
  digitalWrite(dir_b, HIGH); //Set motor direction, 3 high, 4 low

  Serial.println();

  delay(100);
}



