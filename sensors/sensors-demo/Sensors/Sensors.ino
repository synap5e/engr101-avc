//Feel free to use this code.
//Please be respectful by acknowledging the author in the code if you use or modify it.
//Original Author: Bruce Allen
//Modified by: Jason Edwards
//Date: 1/12/10

//Digital pin 7 for reading in the pulse width from the MaxSonar device.
//This variable is a constant because the pin will not change throughout execution of this code.
const int pwPin = 7;
const int DigiIR = 2;
const long SamplePoint = 488/10000;
//variables needed to store values
long pulse, inches, cm;
int object = 0; // variable to store digital value
int analogRange = 0;
long range, IRinches, IRcm;

void setup() {

  //This opens up a serial connection to shoot the results back to the PC console
  Serial.begin(9600);


}

void loop() {

  pinMode(pwPin, INPUT);
  pinMode(DigiIR, INPUT);
  
    //Used to read in the pulse that is being sent by the MaxSonar device.
  //Pulse Width representation with a scale factor of 147 uS per Inch.
  
  range = analogRead(analogRange);
  object = digitalRead(DigiIR);
  pulse = pulseIn(pwPin, HIGH);
  
  //147uS per inch
  inches = pulse/147;
  //change inches to centimetres
  cm = inches * 2.54;

  //convert analog reading into cm and inches
  //5V/1023 = 0.00488v - 4.88mV per sample point
  //Serial.print(range);
  //Serial.print(", ");
  IRcm = range * SamplePoint;
  IRinches = IRcm / 2.54;
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm, ");
  //Serial.println();
  
  if(object == HIGH){
    Serial.print("Path Clear");
  }
  else{
     Serial.print("Object Detected");
  }
  
  if(range > 610){Serial.print(" 3cm");}
  else if(range <= 610 && range > 540){Serial.print(" 4cm");}
  else if(range <= 540 && range > 450){Serial.print(" 5cm");}
  else if(range <= 450 && range > 380){Serial.print(" 6cm");}
  else if(range <= 380 && range > 340){Serial.print(" 7cm");}
  else if(range <= 340 && range > 295){Serial.print(" 8cm");}
  else if(range <= 295 && range > 265){Serial.print(" 9cm");}
  else if(range <= 265 && range > 245){Serial.print(" 10cm");}
  else if(range <= 245 && range > 220){Serial.print(" 11cm");}
  else if(range <= 220 && range > 205){Serial.print(" 12cm");}
  else if(range <= 205 && range > 190){Serial.print(" 13cm");}
  else if(range <= 190 && range > 180){Serial.print(" 14cm");}
  else if(range <= 180 && range > 170){Serial.print(" 15cm");}
  else if(range <= 170 && range > 160){Serial.print(" 16cm");}
  else if(range <= 160 && range > 153){Serial.print(" 17cm");}
  else if(range <= 153 && range > 146){Serial.print(" 18cm");}
  else if(range <= 146 && range > 135){Serial.print(" 19cm");}
  else if(range <= 135 && range > 120){Serial.print(" 20cm");}
  else if(range <= 110 && range > 100){Serial.print(" 25cm");}
  else if(range <= 95 && range > 80){Serial.print(" 30cm");}
  else if(range <= 76 && range > 69){Serial.print(" 35cm");}
  else if(range <= 69 && range > 58){Serial.print(" 40cm");}
  else{Serial.print(" cant read");} 
  

  Serial.println();
  
  
  delay(500);

} 
 
