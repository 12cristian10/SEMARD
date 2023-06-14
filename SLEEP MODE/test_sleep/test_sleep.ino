#include <esp_sleep.h>

RTC_DATA_ATTR int sleepCount = 0;

const int WAKE_INTERVAL = 10; 

void setup() {
  Serial.begin(115200);
  delay(2000);

  sleepCount++;

  Serial.print("Inicio del despertar ");
  Serial.print(sleepCount);
  Serial.println(" veces");

  esp_sleep_enable_timer_wakeup(WAKE_INTERVAL * 1000000);
}

void loop() {

  esp_deep_sleep_start();
}
