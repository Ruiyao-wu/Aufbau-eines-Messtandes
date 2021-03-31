//#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <Servo.h>


//----------current sensor
#define VIN A1 // define the Arduino pin A1 as voltage input (V in)
const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
const int model = 2;   // enter the model number (see below)
float cutOffLimit = 1.01;// set the current which below that value, doesn't matter. Or set 0.5

float sensitivity[] ={
          0.185,// for ACS712ELCTR-05B-T
          0.100,// for ACS712ELCTR-20A-T
          0.066// for ACS712ELCTR-30A-T
         }; 
const float QOV =   0.5 * VCC;      // set quiescent Output voltage of 0.5V
float voltage;                      // internal variable for voltage
//------------------------


//HX711_ADC LoadCell(4, 5); // parameters: dt pin, sck pin<span data-mce-type="bookmark" style="display: inline-block; width: 0px; overflow: hidden; line-height: 0;" class="mce_SELRES_start"></span>
Servo ESC;     // create servo object to control the ESC
int potValue;  // value from the analog pi

void setup() {
  //LoadCell.begin(); // start connection to HX711
  //LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  //LoadCell.setCalFactor(1070); // calibration factor for load cell => strongly dependent on your individual setup
  ESC.attach(9,500,2000); // (pin, min pulse width, max pulse width in microseconds) 
  Serial.begin(9600);
  
}
void loop() {
  float current1 = 0;
  int itter = 200;
  for ( int i = 0; i<itter; i++) {
    delay(1);
    float voltage_raw =   (5.0 / 1023.0)* analogRead(VIN);
    voltage =  voltage_raw - QOV + 0.0018;// 0.000 is a value to make voltage zero when there is no current
    float current = voltage / sensitivity[model];
    current1 = current1 + current;
  }
 Serial.println((current1/itter));
  
  //--------------------current sensor
//  float voltage_raw =   (5.0 / 1023.0)* analogRead(VIN);
//  voltage =  voltage_raw - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
//  float current = voltage / sensitivity[model];
//
//    Serial.println(current); // print the current with 2 decimal places
//
//
//  delay(250);
  //----------------------------------
  
  //LoadCell.update(); // retrieves data from the load cell
  //float i = LoadCell.getData(); // get output value
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
  //Serial.println(i);
  ESC.write(potValue);    // Send the signal to the ESC
  //delay(200);
}
