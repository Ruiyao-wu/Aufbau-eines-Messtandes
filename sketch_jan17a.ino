
#include <Wire.h>
#include <Servo.h>
#include <HX711_ADC.h>
HX711_ADC LoadCell(6, 7); 
void setup() {
  // put your setup code here, to run once:
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(1070);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float kraft = LoadCell.getData();
  Serial.println(kraft);

}
