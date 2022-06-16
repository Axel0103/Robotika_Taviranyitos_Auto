#include <Wire.h>
#include <string.h>
#include <stdio.h>
//#include "Volume.h"

#define SLAVE_ADD  19       // I2C SLAVE PIN
#define ANSWERSIZE 11

#define BalEN  3        // BAL MOTOR ENABLE PIN 
#define BalIN1 6
#define BalIN2 4

#define JobbEN  9        // JOBB MOTOR ENABLE PIN
#define JobbIN1 8
#define JobbIN2 7

#define ECHO  10
#define TRIGGER  11

#define HANGSZORO  5

#define LEDOut1  13
#define LEDOut2  12

long duration;
int distance;

int readUltrasonicDistance()
{
  // Clears the TRIGGER condition
  digitalWrite(TRIGGER, LOW);
  Serial.println("Second delay - microsec");
  //delayMicroseconds(2);
  // Sets the TRIGGER HIGH (ACTIVE) for 10 microseconds
  digitalWrite(TRIGGER, HIGH);
  Serial.println("Third delay - microsec");
  //delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  // Reads the ECHO, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO, HIGH);
  // Calculating the distance  
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);
  pinMode(HANGSZORO, OUTPUT);
  
  pinMode(BalEN, OUTPUT);
  pinMode(JobbEN, OUTPUT);
  
  pinMode(JobbIN1, OUTPUT);
  pinMode(JobbIN2, OUTPUT);
  pinMode(BalIN1, OUTPUT);
  pinMode(BalIN2, OUTPUT);

  pinMode(LEDOut1, OUTPUT);
  pinMode(LEDOut2, OUTPUT);

  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);

  digitalWrite(BalIN1, LOW);
  digitalWrite(BalIN2, LOW);
  digitalWrite(JobbIN1, LOW);
  digitalWrite(JobbIN2, LOW);
}

int headlight = 0;

int direction = 0; //0 - egyenes --- (-1) - hátra --- (+1) - előre
int steering = 0;  //0 - egyenes --- (-1) - balra --- (+1) - jobbra

int joystickLeftRight = 0;
int joystickForwardBackward = 0;

int deadzone = 150;
int soundToneDuration = 0;


void ReadAnswer() {
  Wire.requestFrom(SLAVE_ADD, ANSWERSIZE, false);
  Serial.println("Request received");
  byte bytes[ANSWERSIZE];
  
  int i = 0;
  while(Wire.available() ){
    bytes[i] = Wire.read();
    i++;
  }

  int n = sizeof(bytes);
  char chars[n + 1];
  memcpy(chars, bytes, n);
  chars[n] = '\0';        // Null-terminate the string
  
  String values[3];
  String temp = "";

  int j = 0;
  for(byte i = 0; i < sizeof(chars); i++) {
   if(chars[i] == ';')
   {
     values[j] = temp;
     temp = "";
     j++;
   }
   else
     temp+=chars[i]; 
  }
  values[2] = temp;
  joystickLeftRight = values[0].toInt();
  joystickForwardBackward = values[1].toInt();
  if(values[2] == "y")
  {
    switch(headlight){
      case 1:
        headlight = 0;
        break;
       case 0:
        headlight = 1;
        break;
    }
  }
}

void generateSound() {
    tone(HANGSZORO, 440, 127);
    Serial.println("Forth delay - volume delay");
    Serial.println("SIPOLOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOK");
}

void proximitySensor() {
    if (readUltrasonicDistance() <= 20) {
      generateSound();
    }
    else {
      noTone(HANGSZORO);
    }
}

void loop()
{
  Serial.println("First delay.");
  delay(400);
  Serial.println("First delay over");
  Serial.println();Serial.println();
  ReadAnswer();

  direction = 0;
  steering = 0;
  if (joystickForwardBackward < (512 - deadzone)) {
    direction = -1;
  }
  else if (joystickForwardBackward > (512 + deadzone)) {
    direction = 1;
  }
  else {
    direction = 0;
  }

  Serial.println(joystickLeftRight);
  Serial.println(joystickForwardBackward);

  if (joystickLeftRight < (512 - deadzone)) {
    steering = -1;
  }
  else if (joystickLeftRight > (512 + deadzone)) {
    steering = 1;
  }
  else {
    steering = 0;
  }

  switch(headlight) {
    case 1:
      digitalWrite(LEDOut1, HIGH);
      digitalWrite(LEDOut2, HIGH);
      break;
    case 0:
      digitalWrite(LEDOut1, LOW);
      digitalWrite(LEDOut2, LOW);
      break;
  }
  
  switch(direction) {
    case 0:
      Serial.println("Áll");
      analogWrite(BalEN, 0);     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      analogWrite(JobbEN, 0);    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      break;
    case 1:
      Serial.print("Előre");     
      digitalWrite(BalIN1, HIGH);
      digitalWrite(BalIN2, LOW);
      digitalWrite(JobbIN1, HIGH);
      digitalWrite(JobbIN2, LOW);
      switch(steering){
        case 0:
          Serial.println(" Egyenesen");
          analogWrite(BalEN, map(joystickForwardBackward, (512 + deadzone), 1023, 1, 255));
          analogWrite(JobbEN, map(joystickForwardBackward, (512 + deadzone), 1023, 1, 255));
          break;
        case -1:
          Serial.println(" Balra");
          analogWrite(JobbEN, map(joystickForwardBackward, (512 + deadzone), 1023, 1, 255));
          analogWrite(BalEN, map(joystickLeftRight, 0, (512 - deadzone), 1, 255));
          break;
        case 1:
          Serial.println(" Jobbra");
          analogWrite(BalEN, map(joystickForwardBackward, (512 + deadzone), 1023, 1, 255));
          analogWrite(JobbEN, map(joystickLeftRight, (512 + deadzone), 1023, 255, 1));
          break;
      }
      break;
    case -1:     
      Serial.print("Hátra");
      digitalWrite(BalIN1, LOW);
      digitalWrite(BalIN2, HIGH);
      digitalWrite(JobbIN1, LOW);
      digitalWrite(JobbIN2, HIGH);

      switch(steering){
        case 0:
          Serial.println(" Egyenesen");
          proximitySensor();
          analogWrite(BalEN, map(joystickForwardBackward, (512 - deadzone), 0, 1, 255));
          analogWrite(JobbEN, map(joystickForwardBackward, (512 - deadzone), 0, 1, 255));
          break;
        case -1:
          Serial.println(" Balra");
          proximitySensor();
          analogWrite(JobbEN, map(joystickForwardBackward, (512 - deadzone), 0, 1, 255));
          analogWrite(BalEN, map(joystickLeftRight, 0, (512 - deadzone), 1, 255));
          break;
        case 1:
          Serial.println(" Jobbra");
          proximitySensor();
          analogWrite(BalEN, map(joystickForwardBackward, (512 - deadzone), 0, 1, 255));
          analogWrite(JobbEN, map(joystickLeftRight, (512 + deadzone), 1023, 255, 1));
          break;
      }
      break;
  }
}
