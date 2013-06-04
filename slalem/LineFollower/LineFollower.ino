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

  //delay(500);
  for (int i = 0; i < 200; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }

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
}

int lastError = 0;
float KP = 0.8;
float KD = 15;
#define KI 0.0001

#define M1 100 // left
#define M2 100 // right

#define MAX_SPEED 160
#define SUPER_MAX_SPEED 190
#define MIN_SPEED 25

int max_speed = MAX_SPEED;
int startTime = millis();

unsigned long next = 0;

void loop()
{

  int pos = qtrrc.readLine(sensorValues, QTR_EMITTERS_ON, true);

  // proportional
  int error = ( 2 * (pos - 2500)) / 5;
  
  int derivative = (error - lastError);


  int motorSpeed = KP * error + KD * derivative;
  lastError = error;

  int m1Speed = M1 - motorSpeed;
  int m2Speed = M2 + motorSpeed;



  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;


 /* for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }*/
  Serial.println();
  Serial.print(pos);
  Serial.print("\t");
  Serial.print(error);
  Serial.print("\t");
  Serial.print(m1Speed);
  Serial.print("\t");
  Serial.println(m2Speed);
  Serial.println();

  if (millis() - startTime > 60000) {
    //max_speed = SUPER_MAX_SPEED;
  }


  if (digitalRead(motor_kill)){
    analogWrite(pwm_left, 0);	
    analogWrite(pwm_right, 0);
    delay(100);
  } 
  else {
    Serial.println(m1Speed);
    Serial.println(m2Speed);
    Serial.println();
  //  Serial.println(millis());
   /* if (next <= millis()){
      Serial.println("foo");
       next = millis() + 1000;
       analogWrite(pwm_left, 255);//max(255, m1Speed));	
       analogWrite(pwm_right, 255);//max(255, m2Speed));
       delay(100);
    }*/
    
    analogWrite(pwm_left,  min(max_speed, max(m1Speed, MIN_SPEED)));//max(255, m1Speed));	
    analogWrite(pwm_right, min(max_speed, max(m2Speed, MIN_SPEED)));//max(255, m2Speed));
  }


 /* if (Serial.available() >= 5){
    float p = (Serial.read()-48)*10 + (Serial.read()-48);
    float d = (Serial.read()-48)*100 + (Serial.read()-48)*10 + (Serial.read()-48);

    p/=100;
    d/=10;

    KP = p;
    KD = d;

    Serial.println();
    Serial.print("KP: ");
    Serial.println(KP);
    Serial.print("KD: ");
    Serial.println(KD);



  }*/

  // analogWrite(pwm_left, m1Speed);	
  //  analogWrite(pwm_right, m2Speed);


  //  delay(250);
}




