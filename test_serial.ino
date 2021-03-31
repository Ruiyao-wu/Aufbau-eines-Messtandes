void setup() {
  // put your setup code here, to run once:
pinMode(3,INPUT);
Serial.begin(9600);
}

void loop() {
  delay(1000);
  int xx = digitalRead(3);
  Serial.println(xx);
  // put your main code here, to run repeatedly:

}
