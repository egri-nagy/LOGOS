#include <SPI.h>
#include <GD.h>

#include "random.h"
#include "sprite.h"

#define GRAY(c)  RGB(c,c,c)
#define WHITE RGB(0xff,0xff,0xff)
#define LAG 16
#define PRECALCTAB 81

//to put maximum for letter there
byte letters[4];

byte pointer=0;
byte np = 1;
float xs[PRECALCTAB];
float ys[PRECALCTAB];
float zs[PRECALCTAB]; 
float a = -5.5;
float b = 3.5;
float c = -1;
float xn = 0.1;
float yn = 0;
float zn = 0;
float dt = 0.011; 



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


void textshoot(int n){
  int start = 200 - (n*8); 
  for (int i=0; i<142;i++){
    GD.__wstartspr(0);
    for (byte j=0; j<n; j++){ 
      if (j % 2)
	{
	  draw_sprite(start+(j*16), i, letters[j], 0);
	}
      else
	{
	  draw_sprite(start+(j*16), 282-i, letters[j], 0);
	}

    }  
    GD.waitvblank();  
    GD.__end();
  }  
  delay(999);
  for (int i=0; i<142;i++){
    GD.__wstartspr(0);
    for (byte j=0; j<n; j++){ 
      if (j % 2)
	{
	  draw_sprite(start+(j*16), 142+i, letters[j], 0);
	}
      else
	{
	  draw_sprite(start+(j*16), 142-i, letters[j], 0);
	}

    }  
    GD.waitvblank();  
    GD.__end();
  }  

  GD.__wstartspr(0);
  //parking
  for (byte j=0; j<n; j++){ 
    draw_sprite(401,401,letters[j],0);
  }
  GD.__end();
}


struct voice
{
  float f;
  float a;
} voices[16];

void load()
{
  byte v;
  unsigned int gg = 0;

  float sum = 0.0;
  for (v = 0; v < 16; v++) {
    sum += voices[v].a;
  }
  float scale = 255.0 / sum;
  for (v = 0; v < 16; v++) {
    byte a = int(voices[v].a * scale);
    GD.voice(v, 0, int(4 * voices[v].f), a, a);
  }
}


void note(byte voice, byte m, byte vel)
{
  float f0 = 440 * pow(2.0, (m - 69) / 12.0);
  float a0 = vel / 120.;
  if (voice == 0) {
    float choirA[] = { 3.5, 1.6, .7, 3.7, 1, 2 };
    byte v;
    for (v = 0; v < 6; v++) {
      voices[v].f = (v + 1) * f0;
      voices[v].a = a0 * choirA[v] / 3.7;
    }
  } else {
    voices[voice].f = f0;
    voices[voice].a = a0;
  }
}

static void pause(int n)
{
  load();
  long started = millis();
  while (millis() < (started + n * 3 / 2)) {
  np = (pointer + 1) % PRECALCTAB;
  xn = ys[pointer];
  yn = zs[pointer];
  zn = -a*xs[pointer]-b*ys[pointer]-zs[pointer]+c*pow(xs[pointer],3);
  xs[np] = xs[pointer]+xn*dt;
  ys[np] = ys[pointer]+yn*dt;
  zs[np] = zs[pointer]+zn*dt;
  pointer = np;
  GD.__wstartspr(0);
  draw_sprite(200+(50*xs[(pointer+5*LAG)%PRECALCTAB]), 152+(20*ys[(pointer+5*LAG)%PRECALCTAB]),spL,0);
  draw_sprite(200+(50*xs[(pointer+4*LAG)%PRECALCTAB]), 152+(20*ys[(pointer+4*LAG)%PRECALCTAB]),spO,0);
  draw_sprite(200+(50*xs[(pointer+3*LAG)%PRECALCTAB]), 152+(20*ys[(pointer+3*LAG)%PRECALCTAB]),spG,0);
  draw_sprite(200+(50*xs[(pointer+2*LAG)%PRECALCTAB]), 152+(20*ys[(pointer+2*LAG)%PRECALCTAB]),spO,0);
  draw_sprite(200+(50*xs[(pointer+1*LAG)%PRECALCTAB]), 152+(20*ys[(pointer+1*LAG)%PRECALCTAB]),spS,0);
  GD.__end();
  GD.waitvblank();

//    GD.waitvblank();
  }
}

#define PAUSE(x)      255,x
#define NOTE(v, p, a) v, p, a

static PROGMEM prog_uchar widor_toccata[] = {
#include "music.h"
};

static void play()
{
  prog_uchar *pc = widor_toccata;
  while (pc < (widor_toccata + sizeof(widor_toccata))) {
    byte a = pgm_read_byte_near(pc++);
    byte b = pgm_read_byte_near(pc++);
    if (a == 255) {
      pause(b);
    } else {
      byte c = pgm_read_byte_near(pc++);
      note(a, b, c);
    }
  }
}

void intro(){
  GD.begin();
  GD.ascii();
  char* info[] ={
    "LOGOS proudly presents",
    "MINTRO for FLASHBACK 2012",
    "on the Gameduino shield on top of Arduino board",
    "CPU: Atmega 328 8-bit 16MHz, 32K flash, 2K RAM",
    "Graphic/Sound Chip: Xilinx FPGA",
    "CODE: @",
    "GFX: pfff",
    "Music: @ (yes, it is an available position)",
    "HW support: GNG (giving us an 4:3 VGA monitor)", 
     "______________________________________________",
     "Beat us by coding better intros!! It is easy!!",    
     "______________________________________________",
     "Greetings to lft, luis, jbridon!" 
  };


  delay(200);
   for (int i=0; i<13;i++){
       GD.putstr(0,38 + 2*i, info[i]);

    for (int j = 0; j<16;j++){
      GD.wr16(SCROLL_Y, GD.rd16(SCROLL_Y)+1);
    GD.waitvblank();
    GD.waitvblank();
    }
  }
   for (int j = 0; j<64;j++){
      GD.wr16(SCROLL_Y, GD.rd16(SCROLL_Y)+1);
    GD.waitvblank();  
   }

  delay(2000);
   for (int j = 0; j<512;j++){
      GD.wr16(SCROLL_X, GD.rd16(SCROLL_X)+1);
    GD.waitvblank();  
   }
   delay(2000);

}


void setup(){
  intro();
  GD.begin();

  graypal();
  GD.microcode(random_code, sizeof(random_code));

  GD.copy(PALETTE4A, sprite_sprpal, sizeof(sprite_sprpal));
  GD.uncompress(RAM_SPRIMG, sprite_sprimg);

  // For show, randomly scatter the frames on the screen
  //GD.__wstartspr(0);
  //for (int anim = 0; anim < 256; anim++)
    //draw_sprite(401,401, anim, 0);
  //GD.__end();
  //graypal();
  GD.voice(0,1,81,9,9);
  delay(500);  
  letters[0]=spW;
  letters[1]=spE;
  textshoot(2);
  //delay(500);  
  letters[0]=spH;
  letters[1]=spA;
  letters[2]=spD;
  textshoot(3); 
  //delay(500);  
  letters[0]=spN;
  letters[1]=spO;
  textshoot(2);
  //delay(500);  
  letters[0]=spT;
  letters[1]=spI;
  letters[2]=spM;
  letters[3]=spE;
  textshoot(4); 
  //delay(500);  
  letters[0]=spB;
  letters[1]=spU;
  letters[2]=spT;
  textshoot(3); 
  //delay(500);  
  letters[0]=spW;
  letters[1]=spE;
  textshoot(2);
  //delay(500);  
  letters[0]=spA;
  letters[1]=spR;
  letters[2]=spE;
  textshoot(3); 
  //delay(500);  
  letters[0]=spH;
  letters[1]=spE;
  letters[2]=spR;
  letters[3]=spE;
  textshoot(4); 

  delay(1000);

  
  fade();
  GD.wr(J1_RESET,1);
  GD.fill(RAM_PIC, 0, 1024 * 10);  // Zero all character RAM

  GD.ascii();
  //filling it up with values
  xs[0] = 0.1; //initial x
  ys[0] = 0;
  zs[0] = 0;

  for (byte i=0; i < 99; i++){
    xn = ys[i];
    yn = zs[i];
    zn = -a*xs[i]-b*ys[i]-zs[i]+c*pow(xs[i],3);
    xs[i+1] = xs[i]+xn*dt;
    ys[i+1] = ys[i]+yn*dt;
    zs[i+1] = zs[i]+zn*dt;

  }
  
  GD.putstr(0,0,"Chaotic Trajectory & Folksy Music");  
  GD.copy(PALETTE4A, sprite_sprgreenpal, sizeof(sprite_sprgreenpal));
//GD.uncompress(RAM_SPRIMG, sprite_sprimg);

}

int playcount = 0;
void loop()
{
  if (playcount < 2) {
     play();
     playcount++;
  }
  else{
    GD.begin();
    GD.ascii();
    GD.putstr(0,0,"END OF CARELESSLY CODED 13K INTRO");
  }

}






