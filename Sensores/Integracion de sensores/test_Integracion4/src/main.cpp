#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ticker.h>
#include <WiFi.h>
#include <pubsubclient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
/****************ADS1115**************/
Adafruit_ADS1115 ads; // Objeto para el ADC ADS1115
byte PH_PIN = 0;      // Pin del ADS1115 para el sensor de pH
byte EC_PIN = 1;      // Pin del ADS1115 para el sensor de conductividad
byte OD_PIN = 2;      // Pin del ADS1115 para el sensor de oxigeno disuelto
byte TDS_PIN = 3;     // Pin del ADS1115 para el sensor de tds
/************************************/

/**************sensores***************/
OneWire ds(25);              // Objeto para la comunicación OneWire, conectado al pin 25
DallasTemperature temp(&ds); // Objeto para el sensor de temperatura Dallas DS18B20

int32_t phValues, ecValues, odValues, tdsValues;                 // Variables para sumar los valores de lectura
int32_t avergePh = 0, avergeEc = 0, avergeOd = 0, avergeTds = 0; // Variables para almacenar el promedio de lecturas
int32_t phVoltage, ecVoltage, odVoltage, tdsVoltage;             // Variables para almacenar los valores de voltaje
float TEMP, PH, EC;                                              // Variables para almacenar los valores convertidos de temperatura, ph y conductividad
int16_t TDS, OD;                                                 // Variables para almacenar los valores convertidos de tds y oxigeno disuelto

float readTemperature();
float convertPHUnits(int32_t voltage, float temperature);
float convertECUnits(int32_t voltage, float temperature);
int16_t convertODUnits(int32_t voltage, float temperature);
int16_t convertTDSUnits(int32_t voltage, float temperature);
void readSensors();
/************************************/

/***********************WIFI***********************/
const char *ssid = "SEMARD";
const char *password = "SEMARD123";
WiFiClient espClient;
void wifiConnect();
/*************************************************/

/***********************MQTT***********************/
const char *mqtt_server = "3.88.102.79";
const uint16_t mqtt_port = 1883;
const char *mqtt_client_id = "ttgo_client1";
const char *mqtt_user = "nodo1";
const char *mqtt_password = "semard123";
const char *mqtt_topic = "semard/monitoring/data";
byte mqtt_qos = 1;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length);
void brokerConnect();
/*************************************************/

/***********************GPS***********************/
TinyGPSPlus gps;
HardwareSerial GPS(1);
byte gps_pin_rx = 12;
byte gps_pin_tx = 15;
uint16_t gps_baud = 9600;
void gpsData();
void gpsConect();
/*************************************************/

/*********************jSON******************/
DynamicJsonDocument doc(1024);
char json[1024];
/******************************************/

Ticker readingTicker; // Objeto para programar la lectura de sensores

void setup()
{
  Serial.begin(115200); // Iniciar la comunicación serial

  wifiConnect();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  ads.begin(0x48); // Inicializar el ADC ADS1115 en la dirección 0X48
  temp.begin();    // Inicializar el sensor de temperatura

  GPS.begin(gps_baud, SERIAL_8N1, gps_pin_rx, gps_pin_tx); // Inicializar el GPS

  // Programar la lectura de sensores cada 2 segundos
  readingTicker.attach(2, readSensors); // 2 segundos
}

void loop()
{
  brokerConnect();
  gpsConect();
}

// funcion que lee los valores del sensor de temperatura DS18B20 y retornar la temperatura en grados Celsius
float readTemperature()
{
  temp.requestTemperatures();     // Solicitar la lectura de temperatura al sensor DS18B20
  return temp.getTempCByIndex(0); // Obtener la temperatura en grados Celsius
}

// funcion que convierte los valores de voltaje recibidos a unidades de ph realizando un ajuste con la temperatura recibida
float convertPHUnits(int32_t voltage, float temperature)
{
  float TempCoefficient = 0.1128 * (temperature - 25.0);                // Calcular el coeficiente de temperatura
  return -0.01771 * ((voltage - 1500) / 3) + 8.53543 + TempCoefficient; // Convertir el voltaje del pH a unidades de pH
}

// funcion que convierte los valores de voltaje recibidos a unidades de EC realizando un ajuste con la temperatura recibida
float convertECUnits(int32_t voltage, float temperature)
{
  float ec = 0;
  float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0); // Coeficiente de temperatura
  float CoefficientVoltage = (float)voltage / TempCoefficient; // Voltaje corregido con coeficiente de temperatura

  // Verificar si el voltaje está en el rango adecuado para realizar la conversión
  if (CoefficientVoltage >= 150 || CoefficientVoltage < 3300)
  {
    if (CoefficientVoltage <= 448)
    {
      ec = 6.84 * CoefficientVoltage - 64.32; // Cálculo de EC según la fórmula de ajuste
    }
    else if (CoefficientVoltage <= 1457)
    {
      ec = 6.98 * CoefficientVoltage - 127; // Cálculo de EC según la fórmula de ajuste
    }
    else
    {
      ec = 5.3 * CoefficientVoltage + 2278; // Cálculo de EC según la fórmula de ajuste
    }
  }

  ec /= 1000; // Convertir el resultado a unidades de EC
  return ec;
}

// funcion que convierte los valores de voltaje recibidos a unidades de od realizando un ajuste con la temperatura recibida
int16_t convertODUnits(int32_t voltage, float temperature)
{
  uint16_t saturationVoltage = 243 + (uint16_t)35 * temperature - 24.53 * 35; // Cálculo del voltaje de saturación según la temperatura
  return ((voltage - 20) * 930) / saturationVoltage;                          // Cálculo de OD según la fórmula de ajuste
}

// funcion que convierte los valores de voltaje recibidos a unidades de tds realizando un ajuste con la temperatura recibida
int16_t convertTDSUnits(int32_t voltage, float temperature)
{
  float TempCoefficient = 1.0 + 0.02 * (temperature - 25.0);   // Coeficiente de temperatura
  float CoefficientVoltage = (float)voltage / TempCoefficient; // Voltaje corregido con coeficiente de temperatura

  return (int16_t)(133.42 * CoefficientVoltage * CoefficientVoltage * CoefficientVoltage - 255.86 * CoefficientVoltage * CoefficientVoltage + 857.39 * CoefficientVoltage) * 0.5; // Cálculo de TDS según la fórmula de ajuste
}

// funcion que lee los valores de los sensores y los almacena en las variables correspondientes
void readSensors()
{
  phValues = 0;
  ecValues = 0;
  odValues = 0;
  tdsValues = 0;
  for (int i = 0; i < 8; i++)
  {
    phValues += ads.readADC_SingleEnded(PH_PIN);   // Leer el valor del sensor de pH
    ecValues += ads.readADC_SingleEnded(EC_PIN);   // Leer el valor del sensor de conductividad
    odValues += ads.readADC_SingleEnded(OD_PIN);   // Leer el valor del sensor de oxigeno disuelto
    tdsValues += ads.readADC_SingleEnded(TDS_PIN); // Leer el valor del sensor de tds
    delay(100);
  }
  avergePh = phValues / 8;
  avergeEc = ecValues / 8;
  avergeOd = odValues / 8;
  avergeTds = tdsValues / 8;

  phVoltage = ads.computeVolts(avergePh);   // Convertir el valor del pH a voltaje mV
  ecVoltage = ads.computeVolts(avergeEc);   // Convertir el valor de la conductividad a voltaje mV
  odVoltage = ads.computeVolts(avergeOd);   // Convertir el valor del oxígeno disuelto a voltaje mV
  tdsVoltage = ads.computeVolts(avergeTds); // Convertir el valor de los sólidos disueltos totales a voltaje mV

  TEMP = readTemperature();                    // Obtener la temperatura actual en grados Celsius
  PH = convertPHUnits(phVoltage * 1000, TEMP); // Convertir el voltaje del pH a unidades de pH
  EC = convertECUnits(ecVoltage * 1000, TEMP); // Convertir el voltaje de la conductividad a unidades de ms/cm
  OD = convertODUnits(odVoltage * 1000, TEMP); // Convertir el voltaje del oxígeno disuelto a unidades de mg/l
  TDS = convertTDSUnits(tdsVoltage, TEMP);     // Convertir el voltaje de los sólidos disueltos totales a unidades de ppm

  // Crear el objeto JSON con los valores de los sensores
  doc["temp"] = TEMP;
  doc["ph"] = PH;
  doc["ec"] = EC;
  doc["od"] = OD;
  doc["tds"] = TDS;

  gpsData(); // Leer los datos del GPS y almacenarlos en el objeto JSON

  serializeJson(doc, json);         // Serializar el objeto JSON
  client.publish(mqtt_topic, json); // Publicar el objeto JSON en el tópico "semard/monitoring/data"

  // Imprimir el objeto JSON con los valores medidos
  Serial.println(json);
}

// funcion de conexion a la red WiFi
void wifiConnect()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("conectandose a wifi...");
  }

  Serial.println("conectado a wifi");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// funcion de conexion al broker MQTT
void brokerConnect()
{
  if (!client.connected())

  {
    while (!client.connected())
    {
      Serial.println("conectando a mqtt...");
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_password))
      {
        Serial.println("conectado a mqtt");
      }
      else
      {
        Serial.print("fallo, rc=");
        Serial.print(client.state());
        Serial.println(" intentando de nuevo en 5 segundos");
        delay(5000);
      }
    }
  }
}
// funcion que revisa la conexion del GPS
void gpsConect()
{
  while (GPS.available())
    gps.encode(GPS.read());

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No se detecto el GPS: revisar conexion"));
    return;
  }
}

// funcion que lee los datos del GPS y los almacena en el objeto JSON
void gpsData()
{

  JsonObject posicion = doc.createNestedObject("posicion");
  posicion["latitud"] = gps.location.lat();
  posicion["longitud"] = gps.location.lng();

  char dateStr[11];
  sprintf(dateStr, "%02d/%02d/%04d", gps.date.month(), gps.date.day(), gps.date.year());
  String fecha = String(dateStr);
  doc["fecha"] = fecha;

  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", (gps.time.hour() - 5), gps.time.minute(), gps.time.second());
  String hora = String(timeStr);
  doc["hora"] = hora;
}