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
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/4095.0*3300;  // read the voltage
        phValue = ph.readPH(voltage,25);  // convert voltage to pH with temperature compensation
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
        Serial.println(voltage,2);
        //Serial.println();

    }
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
}

float readTemperature()
{
  tempSensor.requestTemperatures();
  return tempSensor.getTempCByIndex(0);
}
