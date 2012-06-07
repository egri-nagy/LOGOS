#include <SPI.h>
#include <GD.h>

#include "random.h"
#include "sprite.h"

#define GRAY(c)  RGB(c,c,c)
#define WHITE RGB(0xff,0xff,0xff)

void graypal(){
  GD.waitvblank();

  for (int i = 0; i < 256; i++) {
    GD.wr16(RAM_PAL + (4 * i + 0) * 2, GRAY(0));
    GD.wr16(RAM_PAL + (4 * i + 1) * 2, GRAY(0x20));
    GD.wr16(RAM_PAL + (4 * i + 2) * 2, GRAY(0x40));
    GD.wr16(RAM_PAL + (4 * i + 3) * 2, GRAY(0xff));
  }
}



//fading out the character palette
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
  graypal();
  GD.microcode(random_code, sizeof(random_code));

  GD.copy(PALETTE4A, sprite_sprpal, sizeof(sprite_sprpal));
  GD.uncompress(RAM_SPRIMG, sprite_sprimg);

  // For show, randomly scatter the frames on the screen
  GD.__wstartspr(0);
  for (int anim = 0; anim < SPRITE_FRAMES; anim++)
    draw_sprite(random(400), random(300), anim, 0);
  GD.__end();
}  

void loop()
{
  delay(2500);
  GD.waitvblank();

  fade();
}





