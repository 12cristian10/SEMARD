#include <OneWire.h>
#include <DallasTemperature.h>

#define DO_PIN 25
#define TEMP_PIN 33

#define CAL1_V 243 //mv
#define CAL1_T 24 //℃
#define CAL2_V 1300 //mv
#define CAL2_T 15   //℃

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 0, 0, 0, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

OneWire ds(TEMP_PIN); 
DallasTemperature temp(&ds);

float tempC, DO;
uint8_t temperature;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t oxygenD;

float targetDO = 0.81;    // Valor deseado de DO en mg/l
float targetTemp = 24.53; // Valor deseado de temperatura en grados Celsius
int targetVoltage = 330; // Valor deseado de voltaje en mV

void setup()
{
  Serial.begin(115200);
}

void loop()
{
 // Simular los valores deseados
  static float simulatedDO = 0.80;
  static float simulatedTemperature = 24.50;
  static int simulatedvolt = 330;
  simulatedDO += (targetDO - simulatedDO) * 0.01;  // Ajustar la velocidad de cambio según sea necesario
  simulatedTemperature += (targetTemp - simulatedTemperature) * 0.01;  // Ajustar la velocidad de cambio según sea necesario
  simulatedvolt += (targetVoltage - simulatedvolt) * 1;
  // Imprimir los valores simulados
  Serial.print("volt: ");
  Serial.print(simulatedvolt);   
  Serial.print(" mV temp: ");
  Serial.print(simulatedTemperature);    
  Serial.print(" C  DO: ");
  Serial.print(simulatedDO, 2);  
  Serial.println(" mg/l");


  delay(500);
}

int16_t readDO(uint32_t voltage_mv)
{
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * 25 - (uint32_t)CAL1_T * 35;
  return ((voltage_mv-20) * 930 / V_saturation) ;
}


float readTemperature(){
  temp.requestTemperatures();
  return temp.getTempCByIndex(0);
}