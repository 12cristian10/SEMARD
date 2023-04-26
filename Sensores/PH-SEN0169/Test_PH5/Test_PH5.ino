#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define PH_PIN 33
#define TEMP_PIN 25
float voltage,phValue,temperature = 25;
DFRobot_PH ph;
OneWire OneWire(TEMP_PIN);
DallasTemperature tempSensor(&OneWire);

void setup()
{
    Serial.begin(115200);  
    ph.begin();
    tempSensor.begin();
}

void loop()
{
    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  
        timepoint = millis();
        temperature = readTemperature();         
        voltage = analogRead(PH_PIN)/4095.0*3300;  
        phValue = ph.readPH(voltage,25);  
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);

    }
    ph.calibration(voltage,temperature);        
}

float readTemperature()
{
  tempSensor.requestTemperatures();
  return tempSensor.getTempCByIndex(0);
}
