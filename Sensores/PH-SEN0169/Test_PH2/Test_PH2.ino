#define PH_PIN 35
float volt = 0;
float phVal = 0;

void setup() {
  Serial.begin(115200);
  pinMode(adcPin, INPUT);
}

void loop() {
  
  volt = anologRead(PH_PIN);
   
  phVal = 3.5 * volt / 1023 + 0.5;

  Serial.print("PH: ");
  Serial.println(phVal);
  delay(1000);

}
