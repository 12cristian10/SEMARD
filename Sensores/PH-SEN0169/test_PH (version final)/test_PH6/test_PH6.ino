#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_PIN 25
#define PH_PIN 0
#define ADS1115_ADRESS 0X48

Adafruit_ADS1115 ads;
OneWire ds(TEMP_PIN);
DallasTemperature temp(&ds);

const byte numReadings = 20;    
unsigned int AnalogSampleInterval = 25,printInterval = 700,tempSampleInterval=850;  
unsigned int readings[numReadings], i = 0;     
unsigned long AnalogValueTotal = 0;               
unsigned int AnalogAverage = 0,averageVoltage=0;              
unsigned long AnalogSampleTime,printTime,tempSampleTime;
float temperature,PH;

void setup() {
  Serial.begin(115200);
  ads.begin(ADS1115_ADRESS);
  temp.begin();
}

void loop() {
  if(millis()-AnalogSampleTime>=AnalogSampleInterval)
  {
    AnalogSampleTime=millis();
    AnalogValueTotal -= readings[i];
    readings[i] = ads.readADC_SingleEnded(PH_PIN);
    AnalogValueTotal+= readings[i];
    i ++;

    if (i >= numReadings){
      i = 0;
      AnalogAverage = AnalogValueTotal / numReadings;
    }
    
  }

  if(millis()-tempSampleTime>=tempSampleInterval)
  {
    tempSampleTime=millis();
    temperature = readTemperature();
  }

    if(millis()-printTime>=printInterval)
  {
    printTime=millis();
    averageVoltage = ads.computeVolts(AnalogAverage)*1000;
    Serial.print("Voltaje:");
    Serial.print(averageVoltage);  
    Serial.print("mV    ");
    Serial.print("temp:");
    Serial.print(temperature);    
    Serial.print("°C     PH:");

    float TempCoefficient =0.198 * (temperature - 25.0); 
    PH = -0.018 * (((float)averageVoltage- 1500)/3) +6.94 + TempCoefficient;
    PH/=1000;   
        
    Serial.print(PH,2); 
  }
}

float readTemperature(){
  temp.requestTemperatures();
  return temp.getTempCByIndex(0);
}
