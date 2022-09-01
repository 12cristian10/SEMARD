//Librerias necesarias
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/***************LoRa Radio****************/

// SPI LoRa Radio
#define LORA_SCK 5        // GPIO5 - SX1276 SCK
#define LORA_MISO 19     // GPIO19 - SX1276 MISO
#define LORA_MOSI 27    // GPIO27 -  SX1276 MOSI
#define LORA_CS 18     // GPIO18 -   SX1276 CS
#define LORA_RST 14   // GPIO14 -    SX1276 RST
#define LORA_IRQ 26 

//Banda LoRa - ISM en Región 915Mhz
#define BAND 915E6

// Mensaje a enviar por direcciones
byte dir_local   = 0xCD; // Emisor
byte dir_destination = 0xFE; //  Receptor
// identificador de mensaje
byte message_Id = 0XA1;
/****************************************/

/****************Sensores****************/

DHT humSensor(25,DHT11);
OneWire onewire(33);
DallasTemperature tempSensor(&onewire);

//Parametros de los sensores
float hum;
float temp;
/****************************************/

/*************Documento Json*************/

StaticJsonDocument<64> doc;

//Json serializado
char Json[64];
/****************************************/ 

//Variables para usar millis como delay
unsigned long tact;
unsigned long tant = 0;
long tint = 1000; 

//variable para configurar led de la placa
const int indicatorLED = LED_BUILTIN;

void setup() {
  
  Serial.begin(115200);
  //while(!Serial);
  Serial.println("LoRa emisor");
  
  //Configurar conexion
  LoRaInit();

  //Iniciar sensores
  humSensor.begin(); 
  tempSensor.begin();

 //indicador led 
  pinMode(indicatorLED, OUTPUT);

}

void loop() {

 digitalWrite(indicatorLED, HIGH);
 
  readData(hum,temp); // Se leen los datos de los sensores de humedad y temperatura 
  
  // Las siguientes lineas se encargan de enviar cada 10 segundos los datos obtenidos
 tact = millis(); //Leer cuánto tiempo ha transcurrido
 
  if ( (tact-tant) >= tint) //Si la última vez que se ejecutó el if es mayor a 10 segundos, se ejecuta de nuevo el if
  {
    //Guardar los datos en formato Json
    doc["humidity"] = hum; 
    doc["temperature"] = temp;
    
    serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc, se serializa y se guarda en la variable Json

    sendData(dir_destination, dir_local, message_Id, Json); //se crea y envia el paqute  conformado por el archivo y las direcciones del receptor y el emisor
    tant = tact; //Se guarda el tiempo en que se ejecutó el if

    yield();
  
     Serial.print("Paquete enviado: ");
      Serial.print(String(dir_destination,HEX));
      Serial.print(",");
      Serial.print(String(dir_local,HEX));
      Serial.print(",");
      Serial.print(String(message_Id,HEX));
      Serial.print(",");
      Serial.println(Json);
      digitalWrite(indicatorLED, HIGH);
   
    yield(); // procesa lora
  }
  
  
}

//Funcion para configurar e inicializar la libreria LoRa
void LoRaInit(){
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(BAND)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  LoRa.setSpreadingFactor(12); //factor de dispersion
  LoRa.setTxPower(14, PA_OUTPUT_RFO_PIN);
  LoRa.setSyncWord(0xC2);
} 

//Funcion para leer datos de los sensores
void readData(float& hum, float& temp){
  hum = humSensor.readHumidity();
  tempSensor.requestTemperatures();
  temp = tempSensor.getTempCByIndex(0); 
}

//Funcion para enviar los datos de los sensores
void sendData(byte destination, byte sender,byte packetID, String packet){
  
  // espera que el radio esté listo
  // para enviar un paquete
  while(LoRa.beginPacket() == 0){
   
      Serial.println("Esperando radio disponible...");
   
    yield();
    delay(100);
  }
  
  // envio del mensaje LoRa
  LoRa.beginPacket();
  LoRa.write(destination);
  LoRa.write(sender);
  LoRa.write(packetID);
  LoRa.write(packet.length());
  LoRa.print(packet);
  LoRa.endPacket();
}
