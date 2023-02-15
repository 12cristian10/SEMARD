#include <DallasTemperature.h>
#include <OneWire.h>

OneWire onewire(33);
DallasTemperature tempSensor(&onewire);

float temp = 0.0;
unsigned long tact;
unsigned long tant = 0;
long tint = 1000;

void setup() {
  Serial.begin(115200);
  tempSensor.begin();
}

void loop() {

  tact = millis();
  if ((tact-tant) >= tint){

    tempSensor.requestTemperatures();
    temp = tempSensor.getTempCByIndex(0); 
  
    Serial.print("Temperatura: ");
    Serial.println(temp);
    
    tant = tact;
  }
  

}
