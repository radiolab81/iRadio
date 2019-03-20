#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "lcdpcf8574.h"
#include <bcm2835.h>

using namespace std;

#define I2C_DISPLAYADRESSE 0x27

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

int main(int argc, char *argv[]) {
	lcdpcf8574 m(I2C_DISPLAYADRESSE, 0, 0, 0);

	char title[64], title_old[64];
	char now[64], now_old[64];
	char genre[64];
	char bitrate[64];

	FILE *text, *text_now_playing, *text_genre, *text_bitrate;
	// Daemonbetrieb
        while(1) {

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

    		title[22] = '\0';
    		now[22] = '\0';
    		genre[22] = '\0';
    		bitrate[22] = '\0';

    		title[strlen(title)-1]='\0';
    		now[strlen(now)-1]='\0';
    		genre[strlen(genre)-1]='\0';
    		bitrate[strlen(bitrate)-1]='\0';

    		if ( (strcmp(title,title_old)!=0) || (strcmp(now,now_old)!=0) ) {
      			m.lcd_clear();
      			m.lcd_puts(title+1, 0, 0);
      			m.lcd_puts(now+1,1,0);
      			m.lcd_puts(genre+1,2,0);
      			m.lcd_puts(bitrate+1,3,0);
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

