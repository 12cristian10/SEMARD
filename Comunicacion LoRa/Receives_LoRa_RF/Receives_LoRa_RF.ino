/0/Librerias necesarias
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>


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

//instancia de la libreria  LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27,128,32);


//variable para configurar led de la placa
const int indicatorLED = LED_BUILTIN;

void setup() {
   Serial.begin(115200);
  Serial.println("LoRa receptor");
 
  //indicador led 
  pinMode(indicatorLED, OUTPUT);

 //configuracion de display  lcd
  lcd.init();
  lcd.backlight();
   //Configurar conexion
  LoRaInit();

  lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("LoRa iniciado ");
  delay(500);
  //Iniciar el WiFi
  wifiInit();
 delay(500);
}

void loop() {
  
  digitalWrite(indicatorLED, LOW);

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

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Humedad: ");
            lcd.setCursor(8,0);
            lcd.print(hum);
            lcd.setCursor(12,0);
            lcd.print("% ");
            
            lcd.setCursor(0,1);
            lcd.print("Temp:  ");
            lcd.setCursor(5,1);
            lcd.print(temp);
            lcd.setCursor(12,1);
            lcd.print("C");

            serializeJson(doc, Json); //Para poder mandar el formato Json guardado en doc, se serializa y se guarda en la variable Json

            sendDataserver();
      
      digitalWrite(indicatorLED, HIGH);
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
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error al iniciar LoRa");
    while (1);
  }

 LoRa.setSpreadingFactor(12); //factor de dispersion
  LoRa.setTxPower(14, PA_OUTPUT_RFO_PIN);
  LoRa.setSyncWord(0xC2);
} 

//Función para conectarse al WiFi
void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

     lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Conectandose a");
      lcd.setCursor(0,1);
      lcd.print("red  ");
      lcd.setCursor(4,1);
      lcd.print(ssid);
      
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

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Conexion exitosa");
      lcd.setCursor(0,1);
      lcd.print("IP:  ");
      lcd.setCursor(3,1);
      lcd.print(WiFi.localIP());
      
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
