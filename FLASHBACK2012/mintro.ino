#include <SPI.h>
#include <GD.h>

#include "random.h"

#define GRAY(c)  RGB(c,c,c)
#define RED RGB(0xff,0xff,0xff)

void graypal(){
  GD.waitvblank();

  for (int i = 0; i < 256; i++) {
    GD.wr16(RAM_PAL + (4 * i + 0) * 2, GRAY(0));
    GD.wr16(RAM_PAL + (4 * i + 1) * 2, GRAY(0x20));
    GD.wr16(RAM_PAL + (4 * i + 2) * 2, GRAY(0x40));
    GD.wr16(RAM_PAL + (4 * i + 3) * 2, GRAY(0xff));
  }
}

void flash(){
  GD.waitvblank();

  for (int i = 0; i < 256; i++) {
    GD.wr16(RAM_PAL + (4 * i + 0) * 2, RED);
    GD.wr16(RAM_PAL + (4 * i + 1) * 2, RED);
    GD.wr16(RAM_PAL + (4 * i + 2) * 2, RED);
    GD.wr16(RAM_PAL + (4 * i + 3) * 2, RED);
  }
}


void fade(){
  for (byte i = 0; i < 32; i++) {
    for (int j = RAM_PAL; j < (RAM_PAL + 2048); j += 2) {
      uint16_t pal = GD.rd16(j);
      byte r = 31 & (pal >> 10);
      byte g = 31 & (pal >> 5);
      byte b = 31 & pal;
      if (r) r--;
      if (g) g--;
      if (b) b--;
      pal = (r << 10) | (g << 5) | b;
      GD.wr16(j, pal);
    }
  }
}

void setup(){
  GD.begin();
  GD.microcode(random_code, sizeof(random_code));
}  

void loop()
{
  graypal();
  delay(2500);
  GD.waitvblank();

  flash();
  delay(200);
  graypal();

  fade();
//flash();
}





