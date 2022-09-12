//Librerias necesarias
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/***************LoRa Radio****************/

// SPI LoRa Radio
#define LORA_SCK 5       
#define LORA_MISO 19    
#define LORA_MOSI 27   
#define LORA_CS 18     
#define LORA_RST 14   
#define LORA_IRQ 26 

//Banda LoRa - ISM en Región 433Mhz
#define BAND 433E6

// Mensaje a enviar por direcciones
byte dir_local   = 0xCD; //ID de emisor
byte dir_receiver = 0xFE; //ID de receptor
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

void setup() {
  
  Serial.begin(115200);

  Serial.println("LoRa emisor");
  
  //Configurar conexion
  LoRaInit();

  //Iniciar sensores
  humSensor.begin(); 
  tempSensor.begin();
}

void loop() {
 
  readData(hum,temp); // Se leen los datos de los sensores de humedad y temperatura 
  
  // Las siguientes lineas se encargan de enviar cada 10 segundos los datos obtenidos
 tact = millis(); //Leer cuánto tiempo ha transcurrido
 
  if ( (tact-tant) >= tint) //Si la última vez que se ejecutó el if es mayor a 10 segundos, se ejecuta de nuevo el if
  {
    //Guardar los datos en formato Json
    doc["humidity"] = hum; 
    doc["temperature"] = temp;
    
    serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc, se serializa y se guarda en la variable Json

    sendData(dir_receiver, dir_local, Json); //se crea y envia el paqute  conformado por el archivo y las direcciones del receptor y el emisor
    tant = tact; //Se guarda el tiempo en que se ejecutó el if

  
     Serial.print("Paquete enviado: ");
      Serial.print(String(dir_receiver,HEX));
      Serial.print(",");
      Serial.print(String(dir_local,HEX));
      Serial.print(",");
      Serial.println(Json);
    
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
void sendData(byte receiver, byte sender, String packet){
  
  // espera que el radio esté listo
  // para enviar un paquete
  while(LoRa.beginPacket() == 0){
   
      Serial.println("Esperando radio disponible...");
   
    delay(100);
  }
  
  // envio del mensaje LoRa
  LoRa.beginPacket();
  LoRa.write(receiver);
  LoRa.write(sender);
  LoRa.write(packet.length());
  LoRa.print(packet);
  LoRa.endPacket();
}
