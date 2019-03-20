#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wiringPi.h>
#include "ili93xx.h"

FontxFile fxG32[2];
FontxFile fxM32[2];
FontxFile fxG24[2];
FontxFile fxM24[2];
FontxFile fxG16[2];
FontxFile fxM16[2];

#define _DEBUG_ 0

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

int main(int argc, char **argv){
  int i,j;
  uint16_t Y1,Y2;
  uint16_t xpos,ypos;
  uint16_t color;
  uint16_t size;
  unsigned char utf[10];

  int XMAX,YMAX;
  int XMAX2,YMAX2;
  char model[20];

  char title[64], title_old[64];
  char now[64], now_old[64];
  char genre[64];
  char bitrate[64];

  FILE *text, *text_now_playing, *text_genre, *text_bitrate;

  if(wiringPiSetup() == -1) {
    printf("Setup Fail\n");
    return 1;
  }

  char base[128];
  strcpy(base, argv[0]);
  for(i=strlen(base);i>0;i--) {
    if (base[i-1] == '/') {
      base[i] = 0;
      break;
    }
  }
//printf("base=%s\n",base);

  // You can change font file
  // 32Dot Gothic
  char G32[2][128];
  strcpy(G32[0],base);
  strcpy(G32[1],base);
  strcat(G32[0],"fontx/ILGH32XB.FNT");
  strcat(G32[1],"fontx/ILGZ32XB.FNT");
//printf("%s\n",G32[0]);
//printf("%s\n",G32[1]);
  Fontx_init(fxG32,G32[0],G32[1]);

  // 32Dot Mincho
  char M32[2][128];
  strcpy(M32[0],base);
  strcpy(M32[1],base);
  strcat(M32[0],"fontx/ILMH32XF.FNT");
  strcat(M32[1],"fontx/ILMZ32XF.FNT");
//printf("%s\n",M32[0]);
//printf("%s\n",M32[1]);
  Fontx_init(fxM32,M32[0],M32[1]);

  // 24Dot Gothic
  char G24[2][128];
  strcpy(G24[0],base);
  strcpy(G24[1],base);
  strcat(G24[0],"fontx/ILGH24XB.FNT");
  strcat(G24[1],"fontx/ILGZ24XB.FNT");
//printf("%s\n",G24[0]);
//printf("%s\n",G24[1]);
  Fontx_init(fxG24,G24[0],G24[1]);

  // 24Dot Mincho
  char M24[2][128];
  strcpy(M24[0],base);
  strcpy(M24[1],base);
  strcat(M24[0],"fontx/ILMH24XF.FNT");
  strcat(M24[1],"fontx/ILMZ24XF.FNT");
//printf("%s\n",M24[0]);
//printf("%s\n",M24[1]);
  Fontx_init(fxM24,M24[0],M24[1]);

  // 16Dot Gothic
  char G16[2][128];
  strcpy(G16[0],base);
  strcpy(G16[1],base);
  strcat(G16[0],"fontx/ILGH16XB.FNT");
  strcat(G16[1],"fontx/ILGZ16XB.FNT");
  Fontx_init(fxG16,G16[0],G16[1]);

  // 16Dot Mincho
  char M16[2][128];
  strcpy(M16[0],base);
  strcpy(M16[1],base);
  strcat(M16[0],"fontx/ILMH16XB.FNT");
  strcat(M16[1],"fontx/ILMZ16XB.FNT");
  Fontx_init(fxM16,M16[0],M16[1]);

  char ppath[128];
  strcpy(ppath,base);
  strcat(ppath,"pin.conf");


#ifdef ILI9325
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"ILI9325");
  printf("mode:%s\n",model);
  lcdInit(0x9325,XMAX,YMAX,ppath);
#endif

#ifdef ILI9327
  XMAX = 240;
  YMAX = 400;
  strcpy(model,"ILI9327");
  printf("mode:%s\n",model);
  lcdInit(0x9327,XMAX,YMAX,ppath);
#endif

#ifdef SPFD5408
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"SPFD5408");
  printf("mode:%s\n",model);
  lcdInit(0x5408,XMAX,YMAX,ppath);
#endif

#ifdef R61505U
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"R61505U");
  printf("mode:%s\n",model);
  lcdInit(0x1505,XMAX,YMAX,ppath);
#endif

#ifdef ILI9341
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"ILI9341");
#ifndef P16BIT
  printf("mode:%s\n",model);
#else
  printf("mode:%s(16Bit Parallel)\n",model);
#endif
  lcdInit(0x9341,XMAX,YMAX,ppath);
#endif

#ifdef ILI9342
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"ILI9342");
  printf("mode:%s\n",model);
  lcdInit(0x9342,XMAX,YMAX,ppath);
#endif

#ifdef ILI9481
  XMAX = 320;
  YMAX = 480;
  strcpy(model,"ILI9481");
  printf("mode:%s\n",model);
  lcdInit(0x9481,XMAX,YMAX,ppath);
#endif

#ifdef S6D1121
  XMAX = 240;
  YMAX = 320;
  strcpy(model,"S6D1121");
  printf("mode:%s\n",model);
  lcdInit(0x1121,XMAX,YMAX,ppath);
#endif

  XMAX2 = XMAX - 1;
  YMAX2 = YMAX - 1;

  lcdReset();
  lcdSetup();
  lcdDisplayOn();
  lcdFillScreen(BLACK);
  sleep(1);

  // Daemonbetrieb
  while (1) {

       	   	text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r");
		    text_now_playing = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep now_playing:", "r");
	   		text_genre = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep genre:", "r");
    		text_bitrate = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep Bitrate:", "r");
    	
    		fgets(title, 64, text);
    		fgets(now, 64, text_now_playing);
    		fgets(genre, 64, text_genre);
    		fgets(bitrate, 64, text_bitrate);
    	
    		rmSubstr(title,"| title:");
    		rmSubstr(title,"\r");

    		rmSubstr(now,"| now_playing:");
    		rmSubstr(now,"\r");

    		rmSubstr(genre,"| genre:");
    		rmSubstr(genre,"\r");

    		rmSubstr(bitrate,"| Bitrate:");
    		rmSubstr(bitrate,"\r");

    		title[20] = '\0';
    		now[20] = '\0';
    		genre[20] = '\0';
    		bitrate[20] = '\0';

    		title[strlen(title)-1]='\0';
    		now[strlen(now)-1]='\0';
    		genre[strlen(genre)-1]='\0';
    		bitrate[strlen(bitrate)-1]='\0';

    		if ( (strcmp(title,title_old)!=0) || (strcmp(now,now_old)!=0) ) {
			   lcdSetFontDirection(3);
			   lcdDrawFillRect(40, 0, 70, 480, RED);
  			   lcdDrawUTF8String(fxM32, 75, 5, title, YELLOW);	

  			   lcdDrawUTF8String(fxM16, 100, 5, "now playing", BLUE);	
			   lcdDrawFillRect(100, 0, 130, 480, BLACK);
  			   lcdDrawUTF8String(fxM24, 130, 5, now, YELLOW);	

  			   lcdDrawFillRect(140, 0, 170, 480, BLACK);
  			   lcdDrawUTF8String(fxM24, 170, 5, genre, GREEN);
  			   lcdDrawUTF8String(fxM24, 170, 120, bitrate, GREEN);	 
            }

    		strcpy(title_old,title);
    		strcpy(now_old,now);

			if (text != NULL) {
    			pclose(text);
				text = NULL;
			}

			if (text_now_playing != NULL) {
	    		pclose(text_now_playing);
				text_now_playing = NULL;
			}

			if (text_genre != NULL ) {
	    			pclose(text_genre);
					text_genre = NULL;
			}

			if (text_bitrate != NULL ) {
	    			pclose(text_bitrate);
					text_bitrate = NULL;
			}
    		delay(1000);
	}
 return 0;
}
