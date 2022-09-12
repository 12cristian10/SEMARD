//Librerias necesarias
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/Open_Sans_Regular_11.h>
#include "logo.h"


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
byte dirLocal   = 0xFE; //ID de recepotor
byte dirSender =  0xCD; //ID de emisor
int packetStatus;
 int  packetSize;
/****************************************/

/**************Cliente HTTP**************/

//Datos del WiFi
const char* ssid     = "Morelospver";
const char* password = "45452513";

char* server = "https://semardapi.herokuapp.com/";
HTTPClient http;
/****************************************/

/*************Documento Json*************/

StaticJsonDocument<64> doc;

//Almacenar datos recibidos
String packet = "";

//variables deserializadas
float hum;
float temp;

char Json[64];
/****************************************/ 

/****************Display OLED***************/

//parametros de display
#define OLED_WIDTH 128 
#define OLED_HEIGHT 64 
#define OLED_ADRESS 0x3C

//definiendo pines para comunicacion I2C
#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(OLED_WIDTH,OLED_HEIGHT, &Wire, LORA_RST);

/******************************************/

void setup() {
  
   Serial.begin(115200);
   Serial.println("LoRa receptor");

   //Configurar display
   Wire.setPins(OLED_SDA,OLED_SCL);
   Wire.begin(); 
   display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS);
   //display.setFont(&DejaVu_Sans_Mono_10);
   display.setFont(&Open_Sans_Regular_11);
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.clearDisplay(); 
   display.drawBitmap(10,0 , semard, LOGO_WIDTH,LOGO_HEIGHT, WHITE);
   display.display();  
   delay(3000);
 
  //Configurar conexion
  LoRaInit();

  //Iniciar el WiFi
  wifiInit();
}

void loop() {
  
  int messageRec = LoRa.parsePacket();

  if(messageRec !=0){
             receivesData(messageRec);
              
             switch(packetStatus){
                case 1:
                    Serial.println("paquete recibido de manera exitosa ");
                break;
                case 2:
                    Serial.print("Error al recibir paquete: El tamaño enviado no coincide con el recibido ");
                break;
                case 3:
                    Serial.print("Error al recibir paquete : El ID del receptor no coincide con el de este dispositivo ");
                break;
                case 4:
                     Serial.print("Error al recibir paquete : El ID del emisor no coincide con el registrado");
                break;
             }
            Serial.println("Paquete recibido: " + packet);
            Serial.print("'RSSI:  ");
            Serial.println(LoRa.packetRssi());
              
            //(sendReply();
            
            DeserializationError error = deserializeJson(doc, packet);

            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }

            hum =  doc["humidity"];
            temp =  doc["temperature"];

           display.clearDisplay(); 
           display.setCursor(128, 0);
           display.print("Humedad: ");
           display.setCursor(100, 0);
           display.print(hum);
           display.setCursor(128, 10);
           display.print("Temperatura: ");
           display.setCursor(80, 10);
           display.print(temp);
           display.display();
           delay(100);

            
            serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc, se serializa y se guarda en la variable Json

            sendDataserver();
      
  }
  delay(100);
}

void LoRaInit(){
  
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(BAND)) {
    Serial.println("Error al iniciar LoRa");
    
    display.clearDisplay(); 
    display.setCursor(128, 0);
    display.println("Error al iniciar LoRa");
    while (1);
  }

  LoRa.setSpreadingFactor(12); //factor de dispersion
  LoRa.setSyncWord(0xC2);
  LoRa.setTxPower(14, PA_OUTPUT_RFO_PIN);
  
    display.clearDisplay(); 
    display.setCursor(128, 0);
    display.print("LoRa inicado exitosamente");
    display.display();
    delay(1000);
} 

//Función para conectarse al WiFi
void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

    display.clearDisplay(); 
    display.setCursor(128, 0);
    display.print("Conectandose a red "+(String)ssid);
    display.display();
     
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

    display.clearDisplay(); 
    display.setCursor(128, 0);
    display.println("Conexion exitosa ");
    display.println("SSID: "+ (String)WiFi.SSID());
    display.println("IP: "+ (String)WiFi.localIP());
    display.display();
    delay(1000);
      
}

//Funcion para recibir los datos de los sensores
void receivesData(int sizep){

  if(sizep == 0){
      packetStatus = 0; //vacio
      return;
  }

 byte dir_received = LoRa.read();

 if(dir_received == dirLocal){
    byte dir_senderRec = LoRa.read();

    if(dir_senderRec != dirSender){
       packetStatus = 4;  //otro emisor
       return;
    }

    packetSize = LoRa.read();
    packet ="";
    
     while(LoRa.available()){
           packet += (char)LoRa.read();
      }

      if(packetSize != packet.length()){
            packetStatus = 2;   //el tamaño no corresponde al enviado
            return;
      }
      
    packetStatus = 1; //paquete recibido correctamente
  }else{
           packetStatus = 3; //otro destino
            return;
   }  
}

void sendReply(){
   LoRa.beginPacket();
  LoRa.write(dirLocal);
  LoRa.write(dirSender);
  LoRa.write(packet.length());
  LoRa.print("Recibido");
  LoRa.endPacket();
}

void sendDataserver() {
    http.begin(server);
    http.addHeader("Content-Type","application/json");
    
    int response_code = http.POST(Json);
    
    if(response_code > 0){
      Serial.println("Codigo HTTP: "+ String(response_code));
    
       if(response_code == 200){
           String resp = http.getString();
           Serial.println("Respuesta del servidor");
           Serial.println(resp);
       }
    }else{
      Serial.print("Error enviando POST, codigo: ");
      Serial.println(response_code);
    }
    
    http.end();
}
