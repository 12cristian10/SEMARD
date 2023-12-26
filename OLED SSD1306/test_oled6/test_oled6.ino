#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "logo.h"
#include <Fonts\FreeSerifBold9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
 //Wire.setPins(5,4);
  //Wire.begin(5,4);
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  display.clearDisplay(); //for Clearing the display
  //display.setFont(&FreeSerifBold9pt7b);
  display.setTextSize(0.8);
  display.drawBitmap(0,0 , logo, 128, 64, WHITE); 
  //display.setTextSize(0.5); // Draw 2X-scale text
  display.setTextColor(WHITE);
  //display.setCursor(0, 0);
  //display.println("semillero de arduino");
  display.display();      // Show initial text
  //delay(100);
  //display.display();
 delay(3000);
}
void loop() {
  display.clearDisplay(); //for Clearing the display
  display.drawBitmap(63,0 , level_1, 128, 64, WHITE);
  display.setCursor(0, 0);
  display.print("ESTADO:");
  display.setCursor(0, 15);
  display.print("BRAC:250.5");
  display.setCursor(0,31);
  display.print("mg/l");
  display.setCursor(0, 47);
  display.print("BAC:100%"); 
  display.display();
  delay(5000);

  display.clearDisplay(); //for Clearing the display
  display.drawBitmap(63,6 , level_2, 128, 64, WHITE); 
  display.setCursor(0, 0);
  display.print("ESTADO:");
  display.setCursor(0, 15);
  display.print("BRAC:250.5");
  display.setCursor(0,31);
  display.print("mg/l");
  display.setCursor(0, 47);
  display.print("BAC:100%"); 
  display.display();
  delay(5000);

  display.clearDisplay(); //for Clearing the display
  display.drawBitmap(63,6 , level_3, 128, 64, WHITE); 
    display.setCursor(0, 0);
  display.print("ESTADO:");
  display.setCursor(0, 15);
  display.print("BRAC:250.5");
  display.setCursor(0,31);
  display.print("mg/l");
  display.setCursor(0, 47);
  display.print("BAC:100%"); 
  display.display();
  delay(5000);

  display.clearDisplay(); //for Clearing the display
  display.drawBitmap(63,8 , level_4, 128, 64, WHITE); 
    display.setCursor(0, 0);
  display.print("ESTADO:");
  display.setCursor(0, 15);
  display.print("BRAC:250.5");
  display.setCursor(0,31);
  display.print("mg/l");
  display.setCursor(0, 47);
  display.print("BAC:100%"); 
  display.display();
  delay(5000);
}
