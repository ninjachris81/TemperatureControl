#ifndef SSD1306_TEXT_H
#define SSD1306_TEXT_H

#include <Wire.h>

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define WIDTH 128
#define HEIGHT 64

#define LINE_COUNT 8
#define CHAR_BUFFER_SIZE 6
#define CHAR_WIDTH 6
#define MAX_CHARS_IN_LINE 21

class SSD1306Text {
public:
  void begin(uint8_t i2caddr, uint8_t switchvcc = SSD1306_SWITCHCAPVCC);
  
  void setLine(uint8_t lineNo, String text);
  
  void setHorizontalLine(uint8_t lineNo);
  
  static const uint8_t lineCount = LINE_COUNT;
  
  void render();
  
  void clearDisplay();
  
private:
  bool needsRefresh;

  uint8_t i2caddr;
  String lines[LINE_COUNT];

  void ssd1306_command(uint8_t c);

  void drawChar(unsigned char c, uint8_t color, uint8_t bg, uint8_t size, uint8_t thisBuffer[]);

  void drawPixel(int16_t x, int16_t y, uint8_t color, uint8_t thisBuffer[]);
  
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color, uint8_t thisBuffer[]);
  
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color, uint8_t thisBuffer[]);

};


#endif
