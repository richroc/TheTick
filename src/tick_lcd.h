#ifndef TICK_LCD_H
#define TICK_LCD_H

#ifdef USE_LCD
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void display_string(String data){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(data);
  display.display();
}

void display_receivedData(const uint8_t* data, const uint8_t bits, const char* message) {
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(message);
  display.print(bits);
  display.println("bits");

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));

  uint8_t bytes = (bits+7)/8;
  for (int i=0; i<bytes; i++) {
    display.print(data[i] >> 4, 16);
    display.print(data[i] & 0xF, 16);
  }
  display.display();
}

void display_init(){
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display_string(F("MY BODY IS READY"));
}
#else

void display_string(String data){}
void display_receivedData(uint8_t* data, uint8_t bits, const char* message){}
void display_init(){}

#endif
#endif