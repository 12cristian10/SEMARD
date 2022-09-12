//Librerías necesarias
#include <WiFi.h>
#include <HTTPClient.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
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

/*************Documento Json*************/

StaticJsonDocument<64> doc;

//Almacenar datos recibidos
String result = "";
/****************************************/ 

float hum, temp;
void setup() {
  //Iniciar comunicación serial
  Serial.begin(115200);
  
  //Iniciar el WiFi
  wifiInit();
  //Configurar broker MQTT
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop() {
  //Verificar conexión con el broker MQTT
  if (!mqttClient.connected()) {
    reconnect(); //Reconectar en caso de que se desconecte
  }
  //Comprobar si hay mensajes entrates
  mqttClient.loop();

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

void reconnect() {
  //Mientras se conecta el broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    //el ID de este TTGOLoRa32 es "Nodo2"
    if (mqttClient.connect("Nodo2")) {
      
      Serial.println("Conectado");
      
      //Se suscribe al tópico Comunicacion/datos
      mqttClient.subscribe("Comunicacion/datos");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Vaciar la variable del Jsonn serializado
  result = "";
  //Se convierte el mensaje recibido en String
  for (int i=0;i<length;i++) {
    result= result + (char)payload[i];
  }
  //Comando para deserealizar el Json recibido
  DeserializationError error = deserializeJson(doc, result);
  if (error) 
  {
    Serial.print(F("Error al deserializar el documento recibido"));
    Serial.println(error.f_str());
  }
  //Obtener los valores del Json recibido
  hum = doc["humedad"];
  temp = doc["temperatura"];
  //Imprimir el valor del Json en el monitor serial
  Serial.print("humedad: ");
  Serial.println(hum);
  Serial.print("temperatura: ");
  Serial.println(temp);

  HTTPClient http;
  http.begin("url");
  http.addHeader("Content-Type","application/json");
  
  int response_code = http.POST(doc);
  
  if(response_code > 0){
    Serial.println("Codigo HTTP: "+ String(response_code));
  
     if(response_code == 200){
         String resp = http.getString();
         Serial.println("Respuesta del servidor");
     }
  }else{
    Serial.print("Error enviando POST, codigo: ");
    Serial.println(response_code);
  }
  
  http.end();
}
