#include "TimerOne.h"
#include <Wire.h>
#include <Servo.h>

int sensorPin = A5;
int sensorValue = 0;
int counter = 0;
int umdrehungen;
bool b_status = false;
unsigned long time;
unsigned long nextTime;

Servo ESC;     // create servo object to control the ESC
int potValue;  // value from the analog pi

void setup()
{
  ESC.attach(9,500,2000); // (pin, min pulse width, max pulse width in microseconds) 
  Serial.begin(9600);
  Timer1.initialize(100000);
  Timer1.attachInterrupt(timer_isr);
  }

void timer_isr(void){
  umdrehungen=counter/0.1;
  Serial.println (umdrehungen);
  counter=0;
}

void loop()
{
  sensorValue = analogRead(sensorPin);
  //Serial.println(sensorValue);
  if (not b_status)
    if(sensorValue>700){
      counter++;
      b_status=true;
    }  
  else{ 
    if(sensorValue<50){
      b_status=false;
    }
    }
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
  ESC.write(potValue);    // Send the signal to the ESC
   
}
