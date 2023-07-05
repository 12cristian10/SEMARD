#include <OneWire.h>
#include <DallasTemperature.h>

#define TDS_PIN 25
#define TEMP_PIN 33

const int numReadings = 30;
const float offsetVoltage = 0.09;    
int AnalogSampleInterval = 40,printInterval = 800,tempSampleInterval=100;  
int readings[numReadings],copyReadings[numReadings];        
int AnalogAverage = 0, i = 0, j = 0;              
unsigned long AnalogSampleTime,printTime,tempSampleTime;
float temperature=25,tds,averageVoltage=0;
OneWire ds(TEMP_PIN);
DallasTemperature temp(&ds);

void setup()
{
    Serial.begin(115200);
    pinMode(TDS_PIN,INPUT);
    temp.begin();
}

void loop()
{
  if(millis()-AnalogSampleTime>=AnalogSampleInterval)
  {
    AnalogSampleTime=millis();
    readings[i] = analogRead(TDS_PIN);
    i ++;

    if (i >= numReadings){
      i = 0;
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
    averageVoltage= getMedianNum(readings,numReadings) *  3.3 / 4095.0;
    Serial.print("Voltaje:");
    Serial.print(averageVoltage);  
    Serial.print("V    ");
    Serial.print("temp:");
    Serial.print(temperature);    
    Serial.print("^C     TDS:");

    float TempCoefficient=1.0+0.02*(temperature-25.0); 
    float CoefficientVolatge=(averageVoltage - offsetVoltage)/TempCoefficient;
    
    tds=(133.42*pow(CoefficientVolatge,3) - 255.86*pow(CoefficientVolatge,2) + 857.39*CoefficientVolatge)*0.5;
      
    Serial.print(tds,0);
    Serial.println(" ppm");
  }

}

int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

float readTemperature(){
  temp.requestTemperatures();
  return temp.getTempCByIndex(0);
}
