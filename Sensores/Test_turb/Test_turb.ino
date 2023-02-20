byte pin = 35;
int val = 0;
float volt = 0.0;

void setup() {
  Serial.begin(115200); 
}
void loop() {
  val = analogRead(pin);
  volt = val * (5.0 / 1024.0);
  ntu = - (1120.4*volt*volt) + (5742.3*volt)-4352.9;
  Serial.print("Voltaje: ");
  Serial.println(volt); 
  Serial.print("NTU: ");
  Serial.println(ntu); 

  delay(500);
}
