#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_PIN 25

OneWire ds(TEMP_PIN);
DallasTemperature temp(&ds);

const uint32_t READ_TIME = 60000; // 1 minuto en milisegundos
const uint32_t SLEEP_TIME = 300000; // 5 minutos en milisegundos

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  temp.begin();
}

void loop() {
  unsigned long currentMillis = millis();

    temp.requestTemperatures();
    float temperature = temp.getTempCByIndex(0);

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");


  if (currentMillis - previousMillis >= READ_TIME) {
    previousMillis = currentMillis;
    
    Serial.println("Estoy durmiendo");
    esp_deep_sleep(SLEEP_TIME * 1000); // Convertir a microsegundos
  }
}
  