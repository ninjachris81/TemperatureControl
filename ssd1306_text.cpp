#include "ssd1306_text.h"
#include "glcdfont.c"      // @see https://github.com/adafruit/Adafruit_SSD1306

void SSD1306Text::setLine(uint8_t lineNo, String text) {
  if (lineNo > LINE_COUNT-1) return;
  lines[lineNo] = text.length()>MAX_CHARS_IN_LINE ? text.substring(0, MAX_CHARS_IN_LINE) : text;
  needsRefresh = true;
}

void SSD1306Text::clearDisplay() {
  for (uint8_t i=0;i<LINE_COUNT;i++) {
    lines[i] = "";
  }
  needsRefresh = true;
}

void SSD1306Text::begin(uint8_t i2caddr, uint8_t vccstate) {
  this->i2caddr = i2caddr;
  
  needsRefresh = true;

  // I2C Init
  Wire.begin();

  // Init sequence for 128x64 OLED module
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80
  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(0x3F);
  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
  if (vccstate == SSD1306_EXTERNALVCC) 
    { ssd1306_command(0x10); }
  else 
    { ssd1306_command(0x14); }
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1306_EXTERNALVCC) 
    { ssd1306_command(0x9F); }
  else 
    { ssd1306_command(0xCF); }
  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1306_EXTERNALVCC) 
    { ssd1306_command(0x22); }
  else 
    { ssd1306_command(0xF1); }
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}

void SSD1306Text::render() {


  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(WIDTH-1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  ssd1306_command(7); // Page end address

  // I2C
  
  uint8_t charBuffer[CHAR_BUFFER_SIZE];
  
  unsigned long time;
  time = millis();
  
  //Serial.println(F("RENDER"));
  
  for (uint8_t i=0;i<LINE_COUNT;i++) {

    for (uint8_t o=0;o<MAX_CHARS_IN_LINE;o++) {
      memset(charBuffer, 0, CHAR_BUFFER_SIZE);
      
      if (i<lines[i].length()) drawChar(lines[i].charAt(o), WHITE, BLACK, 1, charBuffer);
      
      Wire.beginTransmission(i2caddr);
      WIRE_WRITE(0x40);

      for (uint8_t x=0;x<CHAR_BUFFER_SIZE;x++) {
        WIRE_WRITE(charBuffer[x]);
      }

      Wire.endTransmission();
    }
    
    Wire.beginTransmission(i2caddr);
    WIRE_WRITE(0x40);
    for (uint8_t x=0;x<WIDTH-MAX_CHARS_IN_LINE*CHAR_WIDTH;x++) {
      WIRE_WRITE(0);
    }
    Wire.endTransmission();
  }

/*
  Serial.print(F("RENDER TOOK "));
  Serial.print(millis() - time);
  Serial.println(F(" ms"));
  */
  
  needsRefresh = false;
}

void SSD1306Text::drawChar(unsigned char c, uint8_t color, uint8_t bg, uint8_t size, uint8_t thisBuffer[]) {
  int8_t x = 0;
  int8_t y = 0;
  
  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);

    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color, thisBuffer);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color, thisBuffer);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg, thisBuffer);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg, thisBuffer);
        }
      }
      line >>= 1;
    }
  }

/*
  for (uint8_t i=0;i<CHAR_BUFFER_SIZE;i++) {
    Serial.println(thisBuffer[i], BIN);
  }
  */

}

void SSD1306Text::drawPixel(int16_t x, int16_t y, uint8_t color, uint8_t thisBuffer[]) {
  // x is which column
    switch (color) 
    {
      case WHITE:   thisBuffer[x+ (y/8)*CHAR_WIDTH] |=  (1 << (y&7)); break;
      case BLACK:   thisBuffer[x+ (y/8)*CHAR_WIDTH] &= ~(1 << (y&7)); break; 
      case INVERSE: thisBuffer[x+ (y/8)*CHAR_WIDTH] ^=  (1 << (y&7)); break; 
    }
}

void SSD1306Text::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color, uint8_t thisBuffer[]) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawLine(i, y, h, y+h-1, color, thisBuffer);
  }
}

// Bresenham's algorithm - thx wikpedia
void SSD1306Text::drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint8_t color, uint8_t thisBuffer[]) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color, thisBuffer);
    } else {
      drawPixel(x0, y0, color, thisBuffer);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void SSD1306Text::ssd1306_command(uint8_t c) { 
  // I2C
  uint8_t control = 0x00;   // Co = 0, D/C = 0
  Wire.beginTransmission(i2caddr);
  WIRE_WRITE(control);
  WIRE_WRITE(c);
  Wire.endTransmission();
}
