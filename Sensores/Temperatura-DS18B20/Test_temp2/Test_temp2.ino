#include <DallasTemperature.h>
#include <OneWire.h>

OneWire onewire(25);
DallasTemperature tempSensor(&onewire);

float temp, suma, promTemp, tempCal;
float val[10];
float tempRef = 16.7;
float tempLec = 16.75;
float k = 1.36;
void setup() {
  Serial.begin(9600);
  tempSensor.begin();
}

void loop() {
    
    tempSensor.requestTemperatures();
    temp = tempSensor.getTempCByIndex(0); 
    /*for(int i=0; i<10; i++){
      tempSensor.requestTemperatures();
      val[i] = tempSensor.getTempCByIndex(0); 
      delay(10);
    }*/

    /*for(int i=0; i<9; i++){
      for(int j=i+1; j<10;j++){
         if(val[i]>val[j]){
              temp=val[i];
              val[i]=val[j];
              val[j]=temp;
         }
      }
    }*/

    /*suma = 0;
    for(int i=0; i<10; i++){
      suma+=val[i];
    }*/

    //promTemp=suma/10;
    //tempCal = temp - ((tempRef- tempLec)/k);
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" C");
    delay(1000);

}
