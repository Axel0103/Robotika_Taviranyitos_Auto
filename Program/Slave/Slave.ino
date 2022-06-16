#include <Wire.h>
#define SLAVE_ADD 19
#define ANSWERSIZE 13
#define LedBtn 

#define y 15
#define x 14
#define switch 2

void setup()
{
  Serial.begin(9600);
  Wire.begin(SLAVE_ADD);
  pinMode(A1, INPUT); //Y
  pinMode(A0, INPUT); //x
  pinMode(2, INPUT_PULLUP);
}

int LeftRight = 0;
int ForwardBackward = 0;

void loop()
{ 
  Wire.onRequest(requestValues);
  ForwardBackward =  map(analogRead(y), 0, 1023, 1023, 0);
  LeftRight = analogRead(x);
  delay(1000); // Delay a little bit to improve simulation performance
}

void requestValues() {
  Serial.println("Stepped into requestValues");
  String answer = "";

  String LR = "";
  while(LR.length() < 4 - String(LeftRight).length())
  {
    LR += "0";
  }

  LR+= String(LeftRight);

  String FB = "";
  while(FB.length() < 4 - String(ForwardBackward).length())
  {
    FB += "0";
  }

  FB+= String(ForwardBackward);

  answer += LR;
  answer += ";";
  answer += FB;
  answer += ";";
  if (digitalRead(switch) != HIGH) {
    answer+="y";
  }
  else answer+="n";

  Serial.println(answer);

  byte response[answer.length()];
  for (byte i = 0; i < answer.length(); i++) {
    response[i] = (byte)answer.charAt(i);
  }


  Wire.write(response, sizeof(response));
  Serial.println("Response sent");
  delay(50);
}
