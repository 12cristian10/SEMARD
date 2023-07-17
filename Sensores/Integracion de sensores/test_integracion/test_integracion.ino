#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DTH_Turbidity.h"


Adafruit_ADS1115 ads;  // Objeto para el ADC ADS1115

/****************Sensores****************/
OneWire ds(25);  // Objeto para la comunicación OneWire, conectado al pin 33
DallasTemperature temp(&ds);  // Objeto para el sensor de temperatura Dallas DS18B20

DTH_Turbidity turb(14);  // Objeto para el sensor de turbidez

int32_t phValues, ecValues, odValues, tdsValues;  // Variables para sumar los valores de lectura
int32_t phVoltage, ecVoltage, odVoltage, tdsVoltage;  // Variables para almacenar los valores de voltaje
float TEMP, PH, EC, TURB;  // Variables para almacenar los valores convertidos de temperatura, ph, conductividad y turbidez 
int16_t TDS, OD;  // Variables para almacenar los valores convertidos de tds y oxigeno disuelto 
const uint8_t numReadings = 8;  // Número de lecturas promediadas para obtener un valor
uint8_t cont = 0;  // Contador de lecturas
/****************************************/

uint32_t readingInterval = 7500, initialT = 0;  // Intervalo de lectura de sensores en milisegundos y tiempo inicial


void setup() {
   Serial.begin(115200);  // Iniciar la comunicación serial
  ads.begin(0X48);        // Inicializar el ADC ADS1115 en la dirección 0X48
  temp.begin();           // Inicializar el sensor de temperatura
}

void loop() {
  
  uint32_t currentT = millis();  // Obtener el tiempo actual en milisegundos

  if (currentT - initialT >= readingInterval) {  // Verificar si ha pasado el intervalo de lectura deseado
    
    // Sumar los valores de lectura de los canales del ADS1115
    phValues += ads.readADC_SingleEnded(0);  
    ecValues += ads.readADC_SingleEnded(1);  
    odValues += ads.readADC_SingleEnded(2);  
    tdsValues += ads.readADC_SingleEnded(3); 
    cont++;
    Serial.println(cont);
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
    }

    // Actualizar el tiempo inicial
    initialT = currentT;
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