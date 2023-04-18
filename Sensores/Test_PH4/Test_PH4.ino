#include <OneWire.h>
#include <DallasTemperature.h>

#define PH_PIN 33
#define TEMP_PIN 25
float promVolt,temp,valores[10], offset = 1710, tempAj = 25 ;

OneWire OneWire(TEMP_PIN);
DallasTemperature tempSensor(&OneWire);

void setup() {
  Serial.begin(115200);
  tempSensor.begin();

}

void loop() {

  for(int i=0; i<10; i++){
     valores[i] = analogRead(PH_PIN);
     delay(10);
  }
  
  for(int i = 0; i<9;  i++){

    for(int j = i+1; j<10; j++){
      if(valores[i]>valores[j]){
        temp = valores[i];
        valores[i] = valores[j];
        valores[j] = temp;
      }

    }
  }


  promVolt = 0;
  for(int i = 2; i<8;i++){
    promVolt += valores[i];
  }

  
  float volt = (promVolt/6)* 3.3/4095;

  tempAj = readTemperature();

  float ph = (-0.018* (((volt*1000)- 1500)/3)) +6.94;
  float pH_calibrado = (-0.018 * (((volt * 1000) - offset) / 3) + 6.94) +  tempAj;

  Serial.print("Volt: ");
  Serial.println(volt);
  Serial.print("PH: ");
  Serial.println(ph);
    Serial.print("PH CALIBRADO: ");
  Serial.println(pH_calibrado);

  delay(1000);
}

float readTemperature(){
    tempSensor.requestTemperatures();
    return tempSensor.getTempCByIndex(0);
}
