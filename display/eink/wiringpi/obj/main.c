#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <time.h>
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "EPD_2in9.h"

#include "wiringPi.h"
#include "wiringPiSPI.h"

#include <bcm2835.h>


void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

// Handler damit beim Beenden des Programms die Betriebsspannung im 
// Display abgeschaltet wird! LEBENSZEITVERLAENGERUNG !!!
void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Goto Sleep mode\r\n");
    EPD_Sleep();
    DEV_ModuleExit();

    exit(0);
}

int main (void)
{
    // warte bis vlc,spi... aktiv sind
    delay(3000);

    // Initialisierung
    DEV_ModuleInit();

    // Exception handling: ctrl + c
    signal(SIGINT, Handler);

    if(EPD_Init(lut_full_update) != 0) {
        printf("e-Paper init failed\r\n");
    }

    EPD_Clear();
    DEV_Delay_ms(500);

    // Datenhalter für Sendername, Titel,... aus vlc
    char title[64], title_old[64];
    char now[64], now_old[64];
    char genre[64];
    char bitrate[64];

    FILE *text, *text_now_playing, *text_genre, *text_bitrate;


    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    Paint_NewImage(BlackImage, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

	// Daemonbetrieb
  	while (1) {

		// zyklisch Daten von vlc holen
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

                // haben sich Sendername oder Titel geändert? 
    		if ( (strcmp(title,title_old)!=0) || (strcmp(now,now_old)!=0) ) {
                
  		   // Display aus dem Tiefschlaf holen
		     if(EPD_Init(lut_full_update) != 0) {
		        printf("e-Paper init failed\r\n");
		     }
    		     EPD_Clear();

		     // Auf Displaybuffer schreiben
		     Paint_Clear(WHITE);				
		     Paint_DrawString_EN(10, 10, title+1, &Font20, WHITE, BLACK);
  		     Paint_DrawLine(0, 40, EPD_HEIGHT, 40, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);  
		     Paint_DrawString_EN(20, 60, now+1, &Font16, WHITE, BLACK);
		     Paint_DrawString_EN(20, 80, genre+1, &Font16, WHITE, BLACK);
		     Paint_DrawString_EN(20, 100, bitrate+1, &Font16, WHITE, BLACK);

		     // Display aktualisieren und in den Tiefschlaf legen !!! LEBENSZEITVERLAENGERUNG !!!
                     EPD_Display(BlackImage);
		     EPD_Sleep();
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

    		delay(500);
	}

	// Aufraeumen
    free(BlackImage);
    BlackImage = NULL;
}
