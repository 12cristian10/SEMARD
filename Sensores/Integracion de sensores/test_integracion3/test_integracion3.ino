#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DTH_Turbidity.h"
#include <esp_deep_sleep.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


Adafruit_ADS1115 ads;  // Objeto para el ADC ADS1115

/*******************GPS*******************/
SoftwareSerial gpsSerial(15, 12); // Crea un objeto de SoftwareSerial para la comunicación con el GPS
TinyGPSPlus gps; // Crea un objeto de la librería TinyGPS++
/*****************************************/

/****************Sensores****************/
OneWire ds(33);  // Objeto para la comunicación OneWire, conectado al pin 33
DallasTemperature temp(&ds);  // Objeto para el sensor de temperatura Dallas DS18B20

DTH_Turbidity turb(25);  // Objeto para el sensor de turbidez

int32_t phValues, ecValues, odValues, tdsValues;  // Variables para sumar los valores de lectura
int32_t phVoltage, ecVoltage, odVoltage, tdsVoltage;  // Variables para almacenar los valores de voltaje
float TEMP, PH, EC, TURB;  // Variables para almacenar los valores convertidos de temperatura, ph, conductividad y turbidez 
int16_t TDS, OD;  // Variables para almacenar los valores convertidos de tds y oxigeno disuelto 
const uint8_t numReadings = 8;  // Número de lecturas promediadas para obtener un valor
uint8_t cont = 0;  // Contador de lecturas
/****************************************/

uint32_t readingInterval = 7500, readTime = 900000, sleepTime = 2700000, initialT = 0; // Intervalo de lectura de sensores, tiempo activo, tiempo dormido y tiempo inicial en milisegundos 

void setup() {
  Serial.begin(115200);  // Iniciar la comunicación serial
  gpsSerial.begin(9600); // Inicia la comunicación serial con el GPS
  ads.begin(0X48);        // Inicializar el ADC ADS1115 en la dirección 0X48
  temp.begin();           // Inicializar el sensor de temperatura 

  // Configura los pines de alimentación del GPS
  pinMode(4, OUTPUT); // PWR_CTRL
  pinMode(16, OUTPUT); // GPS_CTRL
  digitalWrite(4, HIGH); // Enciende el GPS
  digitalWrite(16, LOW); // Activa el control del GPS

  delay(2000); // Espera 2 segundos para permitir que el GPS se estabilice

}

void loop() {

  uint32_t currentT = millis(); // Obtener el tiempo actual en milisegundos

  if (currentT - initialT >= readingInterval) {  // Verificar si ha pasado el intervalo de lectura deseado
    
    // Sumar los valores de lectura de los canales del ADS1115
    phValues += ads.readADC_SingleEnded(0);  
    ecValues += ads.readADC_SingleEnded(1);  
    odValues += ads.readADC_SingleEnded(2);  
    tdsValues += ads.readADC_SingleEnded(3); 
    cont++;

    if(cont >= numReadings){ // Verificar si se han realizado suficientes lecturas promediadas

      // Calcular el voltaje de salida de los sensores y convertir a mV
      phVoltage = ads.computeVolts(phValues / numReadings)*1000;
      ecVoltage = ads.computeVolts(ecValues / numReadings)*1000;
      odVoltage = ads.computeVolts(odValues /numReadings)*1000;
      tdsVoltage = ads.computeVolts(tdsValues / numReadings);
      
      TEMP = readTemperature(); // Obtener la temperatura actual en grados Celsius
      PH = convertPHUnits(phVoltage, TEMP);  // Convertir el voltaje del pH a unidades de pH
      EC = convertPHUnits(ecVoltage, TEMP);  // Convertir el voltaje de la conductividad a unidades de ms/cm
      OD = convertPHUnits(odVoltage, TEMP);  // Convertir el voltaje del oxígeno disuelto a unidades de mg/l
      TDS = convertPHUnits(tdsVoltage, TEMP);  // Convertir el voltaje de los sólidos disueltos totales a unidades de ppm
      TURB = turb.readTurbidity();  // Leer el valor de turbidez en unidades de NTU
      
      // Reiniciar la suma de los valores de lectura y el contador de lecturas
      phValues = 0;
      ecValues = 0;
      odValues = 0;
      tdsValues = 0;
      
      cont = 0;

      // Imprimir los valores medidos
      Serial.printf("Temperatura: %.2f °C\tPH: %.2f \tEC: %.2f ms/cm\tOD: %d mg/l\tTDS: %d ppm\tTurbidez: %.2f ntu\n",TEMP,PH,EC,OD,TDS,TURB);

      char gpsData[100]; // Tamaño del buffer ajustable según la longitud del mensaje
  
      getGPSInfo(gpsData, sizeof(gpsData));

      if (gpsData[0] != '\0') {
        Serial.println(gpsData);
      }

    // Actualizar el tiempo inicial
    initialT = currentT;
  }

  if (currentT >= readTime) {
    esp_sleep_enable_timer_wakeup(sleepTime * 1000);  // Habilitar la interrupción del temporizador para despertar del sueño profundo después de un cierto tiempo (en microsegundos)
    esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);  // Configurar el dominio de alimentación RTC_PERIPH para apagarlo durante el sueño profundo
    Serial.flush();  // Vaciar el búfer de la comunicación serial para asegurarse de que todos los datos se hayan enviado
    esp_deep_sleep_start();  // Iniciar el modo de sueño profundo y entrar en el estado de bajo consumo de energía
  }


}

//funcion que lee los valores del sensor de temperatura DS18B20 y retornar la temperatura en grados Celsius
float readTemperature(){
  temp.requestTemperatures();  // Solicitar la lectura de temperatura al sensor DS18B20
  return temp.getTempCByIndex(0);  // Obtener la temperatura en grados Celsius

}

//funcion que convierte los valores de voltaje recibidos a unidades de ph realizando un ajuste con la temperatura recibida
float convertPHUnits(int32_t voltage, float temperature){
  float TempCoefficient = 0.1128 * (temperature - 25.0);  // Calcular el coeficiente de temperatura
  return -0.01771 * ((voltage - 1500) / 3) + 8.53543 + TempCoefficient;  // Convertir el voltaje del pH a unidades de pH
}

//funcion que convierte los valores de voltaje recibidos a unidades de EC realizando un ajuste con la temperatura recibida
float convertECUnits(int32_t voltage, float temperature){
  float ec=0;
float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0); // Coeficiente de temperatura
  float CoefficientVoltage = (float)voltage / TempCoefficient; // Voltaje corregido con coeficiente de temperatura

  // Verificar si el voltaje está en el rango adecuado para realizar la conversión
  if (CoefficientVoltage >= 150 || CoefficientVoltage < 3300) {
    if (CoefficientVoltage <= 448) {
      ec = 6.84 * CoefficientVoltage - 64.32; // Cálculo de EC según la fórmula de ajuste
    } else if (CoefficientVoltage <= 1457) {
      ec = 6.98 * CoefficientVoltage - 127; // Cálculo de EC según la fórmula de ajuste
    } else {
      ec = 5.3 * CoefficientVoltage + 2278; // Cálculo de EC según la fórmula de ajuste
    }
  }

  ec /= 1000; // Convertir el resultado a unidades de EC
  return ec;
}

//funcion que convierte los valores de voltaje recibidos a unidades de od realizando un ajuste con la temperatura recibida
int16_t convertODUnits(int32_t voltage, float temperature){  
  uint16_t saturationVoltage = 243 + (uint16_t)35 * temperature - 24.53 * 35; // Cálculo del voltaje de saturación según la temperatura
  return ((voltage - 20) * 930) / saturationVoltage; // Cálculo de OD según la fórmula de ajuste
}

//funcion que convierte los valores de voltaje recibidos a unidades de tds realizando un ajuste con la temperatura recibida
int16_t convertTDSUnits(int32_t voltage, float temperature){
  float TempCoefficient = 1.0 + 0.02 * (temperature - 25.0); // Coeficiente de temperatura
  float CoefficientVoltage = (float)voltage / TempCoefficient; // Voltaje corregido con coeficiente de temperatura
  
  return (int16_t)(133.42 * CoefficientVoltage * CoefficientVoltage * CoefficientVoltage - 255.86 * CoefficientVoltage * CoefficientVoltage + 857.39 * CoefficientVoltage) * 0.5; // Cálculo de TDS según la fórmula de ajuste
}

//funcion que lee los datos de posicion y tiempo de un gps y los decodifica para ser almacenados en un buffer
void getGPSInfo(char* buffer, size_t bufferSize) {
  buffer[0] = '\0'; // Inicializa el buffer como una cadena vacía
  
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        // Formatea la información del GPS y la guarda en el buffer
        snprintf(buffer, bufferSize, "Latitud: %.6f Longitud: %.6f Fecha: %02d-%02d-%d Hora: %02d:%02d:%02d",
                 gps.location.lat(), gps.location.lng(),
                 gps.date.day(), gps.date.month(), gps.date.year(),
                 gps.time.hour(), gps.time.minute(), gps.time.second());
        break; // Sale del bucle una vez que se ha obtenido la información
      }
    }
  }
}