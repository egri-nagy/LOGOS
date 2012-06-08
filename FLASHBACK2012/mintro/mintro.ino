#include <SPI.h>
#include <GD.h>

#include "random.h"
#include "sprite.h"

#define GRAY(c)  RGB(c,c,c)
#define WHITE RGB(0xff,0xff,0xff)
#define LAG 16

//to put maximum for letter there
byte letters[4];

byte pointer=0;
byte np = 1;
float xs[100];
float ys[100];
float zs[100]; 
float a = -5.5;
float b = 3.5;
float c = -1;
float xn = 0.1;
float yn = 0;
float zn = 0;
float dt = 0.01; 



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
  delay(1500);
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


void setup(){
  GD.begin();
  GD.ascii();
  GD.putstr(0,0,"MINTRO for FLASHBACK 2012");
  GD.putstr(0,2,"CPU: Atmega 328 8-bit, 32K flash, 2K RAM");
  GD.putstr(0,4,"Graphic/Sound Chip: Xilinx FPGA" );

  GD.putstr(0,8,"CODE: @" );
  GD.putstr(0,10,"GFX: pfff" );
  GD.putstr(0,12,"SFX: n/a  ...well @");

  GD.putstr(0,14,"Oldskool VGA monitor by GNG");

  delay(2000);
  for (int i=0; i<200;i++){
    GD.wr16(SCROLL_Y, i);
    GD.waitvblank();
    GD.waitvblank();

  }
  //delay(2000);
  GD.wr16(SCROLL_Y, 0);

  graypal();
  GD.microcode(random_code, sizeof(random_code));

  GD.copy(PALETTE4A, sprite_sprpal, sizeof(sprite_sprpal));
  GD.uncompress(RAM_SPRIMG, sprite_sprimg);

  // For show, randomly scatter the frames on the screen
  GD.__wstartspr(0);
  for (int anim = 0; anim < 256; anim++)
    draw_sprite(401,401, anim, 0);
  GD.__end();
  graypal();

  delay(500);  
  letters[0]=spW;
  letters[1]=spE;
  textshoot(2);
  delay(500);  
  letters[0]=spH;
  letters[1]=spA;
  letters[2]=spD;
  textshoot(3); 
  delay(500);  
  letters[0]=spN;
  letters[1]=spO;
  textshoot(2);
  delay(500);  
  letters[0]=spT;
  letters[1]=spI;
  letters[2]=spM;
  letters[3]=spE;
  textshoot(4); 
  delay(500);  
  letters[0]=spB;
  letters[1]=spU;
  letters[2]=spT;
  textshoot(3); 
  delay(500);  
  letters[0]=spW;
  letters[1]=spE;
  textshoot(2);
  delay(500);  
  letters[0]=spA;
  letters[1]=spR;
  letters[2]=spE;
  textshoot(3); 
  delay(500);  
  letters[0]=spH;
  letters[1]=spE;
  letters[2]=spR;
  letters[3]=spE;
  textshoot(4); 

  delay(1000);

  
  fade();
  GD.wr(J1_RESET,1);
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
  GD.putstr(0,0,"Chaotic Trajectory");  
}

void loop()
{
  np = (pointer + 1) % 100;
  xn = ys[pointer];
  yn = zs[pointer];
  zn = -a*xs[pointer]-b*ys[pointer]-zs[pointer]+c*pow(xs[pointer],3);
  xs[np] = xs[pointer]+xn*dt;
  ys[np] = ys[pointer]+yn*dt;
  zs[np] = zs[pointer]+zn*dt;
  pointer = np;
  GD.__wstartspr(0);
  draw_sprite(200+(50*xs[(pointer+5*LAG)%100]), 152+(20*ys[(pointer+5*LAG)%100]),spL,0);
  draw_sprite(200+(50*xs[(pointer+4*LAG)%100]), 152+(20*ys[(pointer+4*LAG)%100]),spO,0);
  draw_sprite(200+(50*xs[(pointer+3*LAG)%100]), 152+(20*ys[(pointer+3*LAG)%100]),spG,0);
  draw_sprite(200+(50*xs[(pointer+2*LAG)%100]), 152+(20*ys[(pointer+2*LAG)%100]),spO,0);
  draw_sprite(200+(50*xs[(pointer+1*LAG)%100]), 152+(20*ys[(pointer+1*LAG)%100]),spS,0);


  GD.__end();
  GD.waitvblank();

}






