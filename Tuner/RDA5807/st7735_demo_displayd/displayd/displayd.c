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

#include <pthread.h>

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int modus = 0;
// 0 = Internetradio
// 1 = Standby
// 2 = Bluetooth
// 3 = Radio

static void *socket_thread_modiswitch(void *arg) {
	int port = 6030;
  	int r, n;
  	int sockfd, newsockfd, portno;
  	socklen_t clilen;
  	unsigned char buffer[512];
  	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

 	if (sockfd < 0) {
  		perror("ERROR opening socket");
  	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
	}

	bzero(buffer,5);

	fprintf (stderr, "Main socket started on UDP/%d \n", port);

	while((n = read(sockfd,buffer,4)) != 0) {
		printf("%s \n",buffer);
		fflush(stdout);

		if (strcmp((const char *)buffer,(const char *) "stby") == 0 ) {
			fprintf(stderr,"Gehe in Standby.\n");
  			modus=1;
			system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
   		    system("rda5807 mute");

		}

		if (strcmp((const char *)buffer,(const char *) "netr") == 0 ) {
			fprintf (stderr,"Gehe in Internetradiomodus.\n");
			modus=0;
			system("echo \"play\" | nc 127.0.0.1 9294 -w 0");
            system("rda5807 mute");
		}

		if (strcmp((const char *)buffer,(const char *) "blth") == 0 ) {
			fprintf (stderr,"Gehe in Bluetoothmodus.\n");
			modus=2;
			system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
  		    system("rda5807 mute");
		}

        if (strcmp((const char *)buffer,(const char *) "radi") == 0 ) {
            fprintf (stderr,"Gehe in Radiomodus.\n");
            modus=3;
            system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
			system("rda5807 unmute");
        }

 	}

	close(sockfd);
	return 0;
}


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
    // Interprozesskom herstellen
    pthread_t process;
    pthread_create(&process, NULL, socket_thread_modiswitch, NULL);
	pthread_detach(process);

	TFT_ST7735 tft = *new TFT_ST7735(0, 24, 25, 32000000);

	char title[64], title_old[64];
	char now[64], now_old[64];
	char genre[64];
	char bitrate[64];
        char status[255], status_old[255];

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
        // Internetradio
        if (modus == 0) {

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

    		title[12] = '\0';
    		now[20] = '\0';
    		genre[20] = '\0';
    		bitrate[20] = '\0';

    		title[strlen(title)-1]='\0';
    		now[strlen(now)-1]='\0';
    		genre[strlen(genre)-1]='\0';
    		bitrate[strlen(bitrate)-1]='\0';

    		if ( (strcmp(title,title_old)!=0) || (strcmp(now,now_old)!=0) ) {

        		 tft.clearScreen();
                 tft.fillRect(0,0,TFT_height,16,TFT_BLUE);
                 tft.drawString(40,5,"INTERNETRADIO",TFT_WHITE,1);

	 	         tft.drawString(5,30,title+1,TFT_YELLOW,2);
                 tft.drawString(5,50,now+1,TFT_GREEN,1);
                 tft.drawString(5,70,genre+1,TFT_WHITE,1);
                 tft.drawString(5,60,bitrate+1,TFT_WHITE,1);

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

   	      } // ENDE Internetradio


	       // Standbymodus
	       if (modus == 1) {
                strcpy(title,"\0");
                tft.fillRect(0,0,TFT_height,TFT_width,TFT_BLACK);
   	       }

           // Bluetoothmodus
           if (modus == 2) {
		      if (strcmp(title,"BT") != 0) { // zeichne nur einmal, vermeide Displayflackern
                 tft.fillRect(0,0,TFT_height,TFT_width,TFT_BLACK);
                 tft.fillRect(0,0,TFT_height,16,TFT_BLUE);
                 tft.drawString(50,5,"BLUETOOTH",TFT_WHITE,1);
		         tft.drawString(45,50,"BT",TFT_BLUE,6);
		         strcpy(title,"BT\0");
		      }
	      }

           // UKW-Modus
           if (modus == 3) {
		      strcpy(title,"\0");
		      text = popen("rda5807 status", "r");
              fgets(status, 255, text);

		   // Aenderungen im Statusfeld des RDA5807 ?
		   if (strcmp(status_old,status) != 0) {
	   	       strcpy(status_old,status);
                       tft.clearScreen();
                       tft.fillRect(0,0,TFT_height,16,TFT_BLUE);
                       tft.drawString(55,5,"UKW-Radio",TFT_WHITE,1);

     		   // Tokenzerlegung des Statusfeldes des RDA5807
	    	   // Frequenz
		       char* token = strtok(status, " ");
		       tft.drawString(20,40,token,TFT_GREEN,2);
		       tft.drawString(85,40,"MHz",TFT_GREEN,2);

		       int i = 0;

		       while( token != NULL ) {
      	    		token = strtok(NULL, " ");
			        //fprintf(stderr,"%i ist Token: %s\n",i,token);
			        if (i == 1 ) {
				        if (strcmp(token,"stereo," ) == 0)
				           tft.drawString(20,60,"STEREO",TFT_GREEN,1);
			            else
				           tft.drawString(20,60,"MONO",TFT_GREEN,1);
			           }
	        //		if (i == 4 ) ... MUTING
	        //		if (i == 5 ) ... RDA5807 VOLUMELEVEL
			        if (i == 7 ) tft.drawString(20,80,strcat(token,"\0"),TFT_GREEN,1);
			        i++;
   		      } //  while( token != NULL ) {
           } //  (strcmp(status_old,status) != 0) {


          if (text != NULL) {
               pclose(text);
               text = NULL;
          }

	      fprintf(stderr,status_old);
	   }


    	   delay(1000);
	} // while(1) {
}


