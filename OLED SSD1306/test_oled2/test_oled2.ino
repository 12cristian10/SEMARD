#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "logo.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Wire.setPins(21,22);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
     display.clearDisplay(); //for Clearing the display
  display.drawBitmap(10,0 , semard, 112, 64, WHITE); 
 //display.setTextSize(0.5); // Draw 2X-scale text
  //display.setTextColor(SSD1306_WHITE);
 // display.setCursor(15, 40);
  //display.println("semillero de arduino");
  //display.display();      // Show initial text
  //delay(100);
  display.display();
 delay(70);
}
void loop() {
 
}
