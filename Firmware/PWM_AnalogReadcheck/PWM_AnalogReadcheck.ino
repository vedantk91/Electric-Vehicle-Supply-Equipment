#include <Wire.h>
#include <PWM.h>
#define pilot 9 

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
pinMode(pilot,OUTPUT);
InitTimersSafe();
SetPinFrequencySafe(pilot, 1000);
pwmWrite(pilot, 128);

}

void loop() {
  // put your main code here, to run repeatedly:
//   Serial.println(analogRead(A0));
   //delay(50);
}
