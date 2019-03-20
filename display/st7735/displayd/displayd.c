#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "wiringPi.h"
#include "wiringPiSPI.h"

#include "tft_st7735.h"
#include "tft_manager.h"
#include "tft_field.h"
#include <bcm2835.h>


void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}


int main (void)
{
	TFT_ST7735 tft = *new TFT_ST7735(0, 24, 25, 32000000);

	char title[64], title_old[64];
	char now[64], now_old[64];
	char genre[64];
	char bitrate[64];

	FILE *text, *text_now_playing, *text_genre, *text_bitrate;


 	wiringPiSetupGpio();      // initialize wiringPi and wiringPiGpio

	tft.commonInit();         // initialize SPI and reset display
  	tft.initR();              // initialize display
    tft.setRotation(FALSE);
    tft.setBackground(TFT_BLACK);
    tft.clearScreen();


  	tft.setRotation(DEGREE_90);
  	tft.setBackground(TFT_BLACK);
  	tft.clearScreen();        // reset Display

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
				 tft.clearScreen();
			 	 tft.drawString(0,0,title+1,TFT_YELLOW,1);
                 tft.drawString(0,20,now+1,TFT_GREEN,1);
                 tft.drawString(0,40,genre+1,TFT_WHITE,1);
                 tft.drawString(0,60,bitrate+1,TFT_WHITE,1);

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
}


