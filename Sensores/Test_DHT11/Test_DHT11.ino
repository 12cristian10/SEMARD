#include<DHT.h>
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

#define id DHT11
byte sensor = 8;
DHT dht11(sensor,id);
LiquidCrystal_I2C lcd(0x27,16,2);
void setup() {
  Serial.begin(9600);  
  dht11.begin();
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Humedad leida");
  delay(1000);
}

void loop() {
  float humedad = dht11.readHumidity();
  float temperatura = dht11.readTemperature();

  Serial.print("Humedad: ");
  Serial.println(humedad);
  Serial.print("Temperatura: ");
  Serial.println(temperatura);

  lcd.setCursor(0,1);
  lcd.print("Humedad: ");
  lcd.println(humedad);
  
  


}
