
byte adcPin = 35;

void setup() {
  
  Serial.begin(115200);
  pinMode(adcPin, INPUT);

}

void loop() {
  
  int val = analogRead(adcPin);
  float volt = float(val)/1023*5.0;
  Serial.print("PH: ");
  Serial.println(volt);
  delay(1000);

}
