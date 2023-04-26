#define pin A0
void setup() {
  Serial.begin(9600);
}

void loop() {
  int val = analogRead(pin);
  float volt = 5.0 * val / 1023.0;
  float NTU = (-1120.4*pow(volt,2))+ 5742.3*volt-2620.8;
  Serial.print("Voltaje: ");
  Serial.println(volt);
  Serial.print("NTU: ");
  Serial.println(NTU);
  delay(1000);
}
