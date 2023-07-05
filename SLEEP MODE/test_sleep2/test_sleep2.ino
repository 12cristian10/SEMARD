#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_sleep.h>

#define TEMP_PIN 25

OneWire ds(TEMP_PIN);
DallasTemperature temp(&ds);

const uint32_t SLEEP_TIME = 300, READ_TIME = 60, uS_CONVERT_FACTOR = 1000000; 
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

  esp_sleep_enable_timer_wakeup(SLEEP_TIME * uS_CONVERT_FACTOR);
  
}