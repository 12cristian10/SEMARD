

#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define TdsSensorPin 33
#define PhSensorPin 14
GravityTDS gravityTds;
OneWire OneWire(25);
DallasTemperature temp(&OneWire);

//float temperature = 25;
float tdsValue = 0;
float promVolt;
float val[10];

void setup()
{
    Serial.begin(115200);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(4095);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
    temp.begin();
}

void loop()
{
   for(int i=0; i<10; i++){
     val[i] = analogRead(PhSensorPin);
     delay(10);
  }
  
  int t; // variable temporal
  for(int i = 0; i<9;  i++){
    for(int j = i+1; j<10; j++){
      if(val[i]>val[j]){
        t = val[i];
        val[i] = val[j];
        val[j] = t;
      }

    }
  }
  promVolt = 0;
  for(int i = 2; i<8;i++){
    promVolt += val[i];
  }

  float volt = (promVolt/6)* 3.3/4095;

  float ph = (-0.018* (((volt*1000)- 1500)/3)) +6.94;

    temp.requestTemperatures();
    float temperatura = temp.getTempCByIndex(0);
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperatura);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.print("TDS: ");
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    Serial.print("Temperatura: ");
    Serial.print(temperatura,2);
    Serial.println("C");
    Serial.print("PH: ");
    Serial.println(ph);
    delay(1000);

}

 