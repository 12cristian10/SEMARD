#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts\DejaVu_Sans_13Bitmaps.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     9 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  display.clearDisplay(); //for Clearing the display
 display.setFont(&DejaVu_Sans_13Bitmaps);
 display.setTextSize(1);
}

void loop() {
  // put your main code here, to run repeatedly:

}
