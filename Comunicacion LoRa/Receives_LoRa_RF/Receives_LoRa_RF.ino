//Librerias necesarias
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/Open_Sans_SemiBold_12.h>
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
byte dir_local   = 0xFE; // Dispositivo  1
byte dir_sender =  0xCD; // Inicia Remitente

// identificador de mensaje
byte   packetID = 0XA1;
byte   packetstatus = 0;
  // 0:vacio, 1: nuevo, 2:incompleto
  // 3:otro destinatario, 4:Broadcast
  int  packetSize;
/****************************************/

/**************Cliente HTTP**************/

//Datos del WiFi
const char* ssid     = "SEMARD";
const char* password = "SEMARD123";

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
   display.setFont(&Open_Sans_SemiBold_12);
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.clearDisplay(); 
   display.drawBitmap(10,0 , semard, LOGO_WIDTH,LOGO_HEIGHT, WHITE);
   display.display();  
   delay(2000);
 
  //Configurar conexion
  LoRaInit();

  //Iniciar el WiFi
  wifiInit();
}

void loop() {
  
  int messageRec = LoRa.parsePacket();

  if(messageRec !=0){
      receivesData(messageRec);
      
            if (packetstatus == 1 && packetID == 0XA1){
              Serial.println("Paquete recibido: " + packet);
       
            }else{
               Serial.println("Paquete recibido: " + packet);
               Serial.print("Estado del paquete recibido: ");
               Serial.println(packetstatus);
            }

            yield();
            
            DeserializationError error = deserializeJson(doc, packet);

            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              return;
            }

            hum =  doc["humidity"];
            temp =  doc["temperature"];


            serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc, se serializa y se guarda en la variable Json

            sendDataserver();
      
  }/*else{
    Serial.println("No se recibio nada");
             lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Paquetes recibidos: ");
            lcd.setCursor(0,1);
            lcd.print("ninguno ");
  }*/
  delay(100);
  yield();

 
}

void LoRaInit(){
  
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(BAND)) {
    Serial.println("Error al iniciar LoRa");
    
    display.clearDisplay(); 
    display.setCursor(0, 0);
    display.println("Error al iniciar LoRa");
    while (1);
  }

  LoRa.setSpreadingFactor(10); //factor de dispersion
  LoRa.setSyncWord(0xC2);

    display.clearDisplay(); 
    display.setCursor(0, 0);
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
    display.print("Conectandose a red ");
    display.setCursor(119, 0);
    display.print(ssid);
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
    display.print("Conexion exitosa ");
    display.setCursor(0, 10);
    display.print("SSDI: ");
    display.setCursor(98, 10);
    display.print(ssid);
    display.display();
    delay(1000);
      
}

//Funcion para enviar los datos de los sensores
void receivesData(int sizep){

  if(sizep == 0){
      packetstatus = 0;
      return;
  }

 byte dir_received = LoRa.read();

 if(dir_received == dir_local){
    dir_sender = LoRa.read();
    packetID = LoRa.read();
    packetSize = LoRa.read();

    packet ="";
    
     while(LoRa.available()){
           packet += (char)LoRa.read();
      }

      if(packetSize != packet.length()){
            packetstatus = 2; //tamaño incompleto
            return;
      }
      
    packetstatus = 1;
  }else{
           packetstatus = 3; //otro destino
            return;
   }  
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
