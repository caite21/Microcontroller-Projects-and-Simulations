/*
Press the green play button to run the simulation --->
Warning: First time compiling the project may take up to 1 minute.

STM32 Nucleo-L031KB and 2 LCDs with SPI

Description: Draws text and a smiley face using SPI 
  with 2 chip select lines
*/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define tft_DC 2 
#define tft1_CS 3
#define tft2_CS 4

Adafruit_ILI9341 tft1 = Adafruit_ILI9341(tft1_CS, tft_DC);
Adafruit_ILI9341 tft2 = Adafruit_ILI9341(tft2_CS, tft_DC);

void setup() {
  delay(10);
  Serial.begin(115200);
  Serial.println("SPI LCD Displays");

  tft1.begin();
  tft2.begin();

  // Draw text
  tft1.setCursor(42, 120);
  tft1.setTextColor(ILI9341_BLUE);
  tft1.setTextSize(3);
  tft1.println("Hello #1");

  tft2.setCursor(42, 120);
  tft2.setTextColor(ILI9341_YELLOW);
  tft2.setTextSize(3);
  tft2.println("Hello #2");

  tft1.setCursor(36, 160);
  tft1.setTextColor(ILI9341_GREEN);
  tft1.setTextSize(2);
  tft1.println("Nucleo-L031K6");
  
  tft2.setCursor(36, 160);
  tft2.setTextColor(ILI9341_GREEN);
  tft2.setTextSize(2);
  tft2.println("Nucleo-L031K6");

  delay(1500);

  // Draw smiley face
  tft1.fillCircle(40, 40, 15, ILI9341_WHITE);
  tft1.fillCircle(40, 45, 8, ILI9341_BLACK);
  tft1.fillCircle(90, 40, 15, ILI9341_WHITE);
  tft1.fillCircle(90, 45, 8, ILI9341_BLACK);

  int x1=30, y1=75;
  for (int i = 3; i >= -3; i--) {
    tft1.drawLine(x1, y1, x1+10, y1+(2*i), ILI9341_WHITE);
    x1 = x1 + 10;
    y1 = y1 + (2*i);
  }

}

void loop() {
  delay(10);
}
