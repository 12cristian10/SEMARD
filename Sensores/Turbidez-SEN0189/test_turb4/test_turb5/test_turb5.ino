cc#include <Wire.h>

#define TURB_PIN 25

const float CALIBRATION_VALUE = 0.5, NTU_MAX = 3000;
float refVal = 4095, refVolt = 3.3, ntu;
int digitalVal;
void setup() {
  Serial.begin(115200); 
  pinMode(TURB_PIN, INPUT); 
}

void loop() {

  digitalVal = digitalRead(TURB_PIN);
  
  ntu = map(digitalVal, 0, refVal, 0, NTU_MAX) * CALIBRATION_VALUE;
  
  Serial.print("Valor: ");
  Serial.println(digitalVal);
  Serial.print("NTU: ");
  Serial.println(ntu);
  
  delay(1000); 
}
