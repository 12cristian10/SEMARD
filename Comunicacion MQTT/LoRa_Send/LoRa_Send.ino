//Librerías necesarias
#include <WiFi.h>
//#include<ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/**************Cliente MQTT**************/
WiFiClient ttgoLoraClient; 
PubSubClient mqttClient(ttgoLoraClient);


//Datos del WiFi
const char* ssid     = "Morelospver";
const char* password = "45452513";

//Datos del Broker MQTT
char* server = "broker.emqx.io";
int port = 1883;
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
char Json[40];
/****************************************/ 

//Variables para usar millis como delay
unsigned long tact;
unsigned long tant = 0;

void setup() {
  //Iniciar comunicación serial
  Serial.begin(115200);
  
  //Iniciar el WiFi
  wifiInit();
  //Configurar broker MQTT
  mqttClient.setServer(server, port);
//  mqttClient.setCallback(callback);
}

void loop() {
  //Verificar conexión con el broker MQTT
  if (!mqttClient.connected()) {
    reconnect();
  }

  //Comprobar si hay mensajes entrates
  mqttClient.loop();
  
  //Se hace la lectura del valor de los sensores
  readData(hum,temp);
  
 // Las siguientes lineas se encargan de enviar cada 2 segundos los datos obtenidos
 tact = millis(); //Leer cuánto tiempo ha transcurrido
 
  if ( (tact-tant) >= 2000) //Si la última vez que se ejecutó el if es mayor a 2 segundos, se ejecuta de nuevo el if
  {
    //Guardar los datos en formato Json
    doc["humidity"] = hum; 
    doc["temperature"] = temp;
    
    serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc2, se serializa y se guarda en la variable Json
    mqttClient.publish("Comunicacion/datos",Json); //Se publica el Json serializado en el tópico ESP1/Val
    tant = tact; //Se guarda el tiempo en que se ejecutó el if
    Serial.print("Json enviado: ");
    Serial.println(Json); //Se imprime el Json enviado en el monitor serial
  }

}

//Función para conectarse al WiFi
void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print("*");
        delay(500);  
    }
  Serial.println("\n------------------------------------");
  Serial.print("Conectado a la red WiFi: ");
  Serial.println(WiFi.SSID());
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("------------------------------------");  
}

//Función para leer los datos de los sensores
void readData(float& hum, float& temp){
  hum = humSensor.readHumidity();
  tempSensor.requestTemperatures();
  temp = tempSensor.getTempCByIndex(0); 
}

void reconnect() {
  //Mientras se conecta el broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    //el ID de este TTGOLoRa32 es "Nodo1"
    if (mqttClient.connect("Nodo1")) {
      
      Serial.println("Conectado");
    }
  }
}
