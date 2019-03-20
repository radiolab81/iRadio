#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "oled96.h"
#include <bcm2835.h>

extern unsigned char ucSmallFont[];

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}


int main(int argc, char *argv[]) {
	int i;

	char title[64], title_old[64];
	char now[64], now_old[64];
	char genre[64];
	char bitrate[64];

	FILE *text, *text_now_playing, *text_genre, *text_bitrate;

	i=oledInit(1, 0x3c, 0, 0); // for Raspberry Pi, use channel 1
	if (i == 0)
	{
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
			 oledFill(0); // fill with black
			 oledWriteString(0,1,title+1,FONT_NORMAL);
                         oledWriteString(0,2,now+1,FONT_NORMAL);
                         oledWriteString(0,3,genre+1,FONT_NORMAL);
                         oledWriteString(0,4,bitrate+1,FONT_NORMAL);

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
  	 oledShutdown();
	}
   return 0;
} /* main() */

