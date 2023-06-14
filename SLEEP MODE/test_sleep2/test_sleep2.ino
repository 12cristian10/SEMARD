#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_sleep.h>

#define TEMP_PIN 25

OneWire ds(TEMP_PIN);
DallasTemperature temp(&ds);

const uint32_t SLEEP_TIME = 300, READ_TIME = 60, uS_CONVERT_FACTOR = 1000000, mS_CONVERT_FACTOR = 1000; 
unsigned long currentTime = 0, previousTime = 0; 
void setup() {
  
  Serial.begin(115200);
  temp.begin();
}

void loop() {
  
  currentTime = millis();


    temp.requestTemperatures();
    float temperature = temp.getTempCByIndex(0);

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C"); 


  if(currentTime >= SLEEP_TIME * mS_CONVERT_FACTOR){
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * uS_CONVERT_FACTOR);
    esp_deep_sleep_start();
  }
  
}