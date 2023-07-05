#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ADS1X15.h>

#define TDS_PIN 0
#define ADS1115_ADDRESS 0x48
#define TEMP_PIN 33

const byte numReadings = 20;    
const float offsetVoltage = 0.38;
unsigned int AnalogSampleInterval = 25,printInterval = 700,tempSampleInterval=850;  
unsigned int readings[numReadings], i = 0;     
unsigned long AnalogValueTotal = 0;               
unsigned int AnalogAverage = 0;              
unsigned long AnalogSampleTime,printTime,tempSampleTime;
float temperature,tds, averageVoltage=0;

OneWire ds(TEMP_PIN); 
DallasTemperature temp(&ds);
Adafruit_ADS1115 ads;

void setup() {

  Serial.begin(115200);
  Wire.begin();
  ads.begin(ADS1115_ADDRESS);
  temp.begin();
}
void loop() {

  if(millis()-AnalogSampleTime>=AnalogSampleInterval)
  {
    AnalogSampleTime=millis();
    AnalogValueTotal -= readings[i];
    readings[i] = ads.readADC_SingleEnded(TDS_PIN);
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
    averageVoltage = ads.computeVolts(AnalogAverage);
    
    float TempCoefficient=1.0+0.02*(temperature-25.0); 
    float CoefficientVolatge=(float)(averageVoltage- offsetVoltage)/TempCoefficient;
    
    Serial.print("Voltaje:");
    Serial.print(CoefficientVolatge);  
    Serial.print("V    ");
    Serial.print("temp:");
    Serial.print(temperature);    
    Serial.print("^C     TDS:");

    if(CoefficientVolatge <= offsetVoltage){
      tds = 0;
    }else{
      tds=(133.42*pow(CoefficientVolatge,3) - 255.86*pow(CoefficientVolatge,2) + 857.39*CoefficientVolatge)*0.5;
    }
    
    Serial.print(tds,0);
    Serial.println(" ppm");

  }

}

float readTemperature(){
  temp.requestTemperatures();
  return temp.getTempCByIndex(0);
}