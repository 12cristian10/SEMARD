//Programa realizado por Super AgroFighter para el canal
//de youtube de Agricultura Electrónica

//https://youtu.be/920XjcNLM6Y

//Suscribirse, que no cuesta naaaaaa  :D


////////////////////////////////para la pantalla LCD
#include <LiquidCrystal_I2C.h> // Debe descargar la Libreria que controla el I2C
LiquidCrystal_I2C lcd(0x27,16,2); //SI no va, se puede poner 0x20 para probar
//el modulo es de 16 por 2. el protocolo es 0x27. si no va, probar con 0x20

//Pines del módulo I2C
//VCC GND SON LA TENSIÓN Y LA TIERRA
//SDA VA AL A4
//SCL VA AL A5

int sensor = 2; //Pin 2, entrada sensro DHT11
int ventilador = 13; //Salida pin 13 al ventilador



void setup() {
  Serial.begin(9600);
 
  pinMode(ventilador, OUTPUT);

  //Para la pantalla LCD
  lcd.init(); //Inicializamos el LCD
  lcd.backlight(); //Activamos la luz de fondo
  lcd.clear(); //Limpiamos lo que haya en pantalla
  lcd.setCursor(0,0);//Iniciamos el cursor en el punto 0,0
}

void loop() {

 //Para mostrarlo en el LCD
 lcd.setCursor(0,0);//Posicion: columna cero fila cero
 lcd.print("Ventilador activo.  ");
 lcd.print("%");
 lcd.setCursor(0,1);//Posición: olumna 0 fila 1, para que escriba en la segunda linea
 lcd.print("Temperatura: ");
 lcd.print("C");

  //esto si quiero que la pantalla vaya de izquierda a derecha.
  for(int c=0;c<12;c++){ //hace una secuencia hacia la izquierda y luego hacia la derecha, por 12 pasos
  lcd.scrollDisplayLeft();
  delay(600);   }
  for(int c=0; c<12;c++){
  lcd.scrollDisplayRight();
  delay(600);   } 
  lcd.clear();//blanqueamos la pantalla
  }
 
 
