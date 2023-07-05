#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ADS1X15.h>

#define EC_PIN 0
#define ADS1115_ADDRESS 0x48
#define TEMP_PIN 33

const byte numReadings = 20;    
unsigned int AnalogSampleInterval = 25,printInterval = 700,tempSampleInterval=850;  
unsigned int readings[numReadings], i = 0;     
unsigned long AnalogValueTotal = 0;               
unsigned int AnalogAverage = 0,averageVoltage=0;              
unsigned long AnalogSampleTime,printTime,tempSampleTime;
float temperature,EC;

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
    readings[i] = ads.readADC_SingleEnded(EC_PIN);
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
    Serial.print("^C     EC:");

    float TempCoefficient=1.0+0.0185*(temperature-25.0); 
    float CoefficientVolatge=(float)averageVoltage/TempCoefficient;
    if(CoefficientVolatge<150){
      Serial.println("No solution!");
    }else if(CoefficientVolatge>3300){
      Serial.println("Out of the range!"); 
    }else{
      if(CoefficientVolatge<=448){
        EC=6.84*CoefficientVolatge-64.32;
      }else if(CoefficientVolatge<=1457){
        EC=6.98*CoefficientVolatge-127;
      }
      else{
        EC=5.3*CoefficientVolatge+2278; 
        EC/=1000;   
        
      } 
      Serial.print(EC,2); 
      Serial.println("ms/cm");
    }
  }

}

float readTemperature(){
  temp.requestTemperatures();
  return temp.getTempCByIndex(0);
}