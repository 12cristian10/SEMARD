#include <TinyGPS++.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// Define los pines de comunicación con el GPS
#define GPS_SERIAL_RX 15
#define GPS_SERIAL_TX 12

SoftwareSerial gpsSerial(GPS_SERIAL_RX, GPS_SERIAL_TX); // Crea un objeto de SoftwareSerial para la comunicación con el GPS
TinyGPSPlus gps; // Crea un objeto de la librería TinyGPS++

void setup() {
  Serial.begin(115200); // Inicia la comunicación serial con el monitor serie
  gpsSerial.begin(9600); // Inicia la comunicación serial con el GPS

  // Configura los pines de alimentación del GPS
  pinMode(4, OUTPUT); // PWR_CTRL
  pinMode(16, OUTPUT); // GPS_CTRL
  digitalWrite(4, HIGH); // Enciende el GPS
  digitalWrite(16, LOW); // Activa el control del GPS

  delay(2000); // Espera 2 segundos para permitir que el GPS se estabilice
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        // Obtiene la posición del GPS
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        uint16_t year =gps.date.year();
        uint8_t month =gps.date.month();
        uint8_t day =gps.date.day(); 
        uint8_t h =gps.time.hour(); 
        uint8_t m =gps.time.minute(); 
        uint8_t s =gps.time.second(); 
        // Muestra la posición en el monitor serie
        Serial.print("Latitud: ");
        Serial.println(latitude, 6);
        Serial.print("Longitud: ");
        Serial.println(longitude, 6);
        Serial.print("Fecha: ");
        Serial.printf("%d - %d - %d \n",day,month,year);
        Serial.print("Hora: ");
        Serial.printf("%d : %d : %d \n",h,m,s);
      }
    }
  }
}
