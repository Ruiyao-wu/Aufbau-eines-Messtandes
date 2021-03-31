
#include <Wire.h>
#include <Servo.h>
#include <HX711_ADC.h>




//----------Drehzahlwerte
int werte[]={0, 6};//{0, 10,12,15,18,20,23,25,28,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,110,120,130,140,150,160,170,175,100}; //Die Servo-Libary erwartet werte zwischen 0 und 180
int anzahl_werte=2;
int i=0;
//----------

//----------Motoransteuerung
Servo ESC;     // create servo object to control the ESC
//----------

//----------Kraftmessung
HX711_ADC LoadCell_1(6, 7); // parameters: dt pin, sck pin<span data-mce-type="bookmark" style="display: inline-block; width: 0px; overflow: hidden; line-height: 0;" class="mce_SELRES_start"></span>
HX711_ADC LoadCell_2 (9, 10); // parameters: dt pin, sck pin<span data-mce-type="bookmark" style="display: inline-block; width: 0px; overflow: hidden; line-height: 0;" class="mce_SELRES_start"></span>
float kraft_ref=0;
float kraft_act=0;
//----------

//----------Drehzahlmessung
float counter_schranke = 0;
float counter_elek = 0;
bool status_schranke = false;
int drehzahl_schranke=0;
int drehzahl_elek=0;
int interruptPin_elek = 3;
unsigned long drehzahl_ref;
unsigned long programm_ref;
unsigned long zeit_act;
//----------

//----------Stromsensor
#define VIN A3 // define the Arduino pin A3 as voltage input (V in)
//bool initialized=false;
float current_offset=0;
float current_ref= 0;
float current_act=0;
int current_counter=0;
const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
const int model = 2;   // enter the model number (see below)
float cutOffLimit = 0.5;// set the current which below that value, doesn't matter. Or set 0.5
float sensitivity[] ={
          0.185,// for ACS712ELCTR-05B-T
          0.100,// for ACS712ELCTR-20A-T
          0.066// for ACS712ELCTR-30A-T
         }; 
const float QOV =   0.5 * VCC;      // set quiescent Output voltage of 0.5V
float voltage;                      // internal variable for voltage
//------------------------






void setup()
{
  //----------Initialsiere Kraftmessung
  LoadCell_1.begin(); // start connection to HX711
  LoadCell_1.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell_1.setCalFactor(1077); // calibration factor for load cell => strongly dependent on your individual setup
  
  LoadCell_2.begin(); // start connection to HX711
  LoadCell_2.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell_2.setCalFactor(736); // calibration factor for load cell => strongly dependent on your individual setup
  //----------

  //----------Initialisiere Motoransteuerung
  ESC.attach(5,500,2000); // (pin, min pulse width, max pulse width in microseconds)
  ESC.write(0);    // Setze den Motor auf 0
  delay(500);
  //----------
  
  //----------Initialisiere Drehzahlauswertung elektrisch
  pinMode(interruptPin_elek, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin_elek), interrupt_elek , RISING);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), interrupt_schranke , RISING);
  //----------

  Serial.begin(9600);
  //Serial.println("Start");
  drehzahl_ref=millis()+1000;
  programm_ref=millis()+4000;
  }




void loop()
{
  //Es wird über die verschiedenen Drehzahlwerte iteriert. Wurden alle Drehzahlwerte angefahren, 
  //wird der Motor auf 0 gesetzt. Nach jeder Drehzahlveränderung wird 6 sec. gewartet.
  if (i<anzahl_werte)
  {
    zeit_act=millis();
    LoadCell_1.update(); // retrieves data from the load cell
    float kraft = LoadCell_1.getData(); // get output value

    LoadCell_2.update(); // retrieves data from the load cell
    float kraft_schub = LoadCell_2.getData(); // get output value

    float voltage_raw =   (5.0 / 1024.0)* analogRead(VIN);
    voltage =  voltage_raw - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
    float current = voltage / sensitivity[model];
    current_ref=current_ref+current;
    kraft_ref=kraft_ref+kraft;
    current_counter++;


    //DREHZAHLERFASSUNG
    //----------Zählerauswertung
    if (zeit_act>drehzahl_ref)
    {
      drehzahl_elek = (counter_elek*60)/7;       
      drehzahl_schranke=counter_schranke*30;
      current_act=(current_ref/current_counter)-0.1;
      kraft_act=(kraft_ref/current_counter);
      
//      if(i==0 and not initialized){
//        current_offset=current_act;
//        initialized=true;
//      }

      kraft_ref=0;
      current_ref=0;
      counter_elek=0;
      counter_schranke=0;
      current_counter=0;
      
      
      drehzahl_ref=millis()+1000;
    }



    //MESSWERTAUSGABE
    if (zeit_act>programm_ref)//Nach Ablauf von 5 sec werden die neuen Messwerte ausgegeben
    {
      //Serial.println(werte[i]);
      //MOMENTERFASSUNG
      //----------Strommessung
      Serial.print(current_act); // print the current with 2 decimal places
      Serial.print(',');
      //----------

      //----------Kraftmessung
      Serial.print(kraft_act);
      Serial.print(',');
      //----------

      //DREHZAHLERFASSUNG
      //----------Drehzahl elektrisch
      Serial.print(drehzahl_elek);
      Serial.print(',');
      
      //----------Drehzahl Lichtschranke
      Serial.print(drehzahl_schranke);
      Serial.print(',');
      //----------

      //----------Drehzahl Lichtschranke
      Serial.print(kraft_schub);
      Serial.print(',');
      //----------

      Serial.println(kraft);
      
      i=i+1;
      programm_ref=millis()+4000;//Setze neue Programmzeitreferenz (aktuell + 5sec)
    } 
    ESC.write(werte[i]);    // Setze den Motor auf den i-ten Drehzahlwert  
    
  }
  else
  {
    ESC.write(0); //Nach Beendigung des automatischen Programmablaufs setze den Motor auf Null!
  }
    
}


//DREHZAHLERFASSUNG elektrisch: Interruptroutine
void interrupt_elek() {
  counter_elek++;
}
//----------

void interrupt_schranke(){
  counter_schranke++;
}
