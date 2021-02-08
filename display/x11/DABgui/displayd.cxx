#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unistd.h>

#include <ctime>

using namespace std;

#define SKALE_HOEHE 320
#define SKALE_BREITE 480

#define BILDER_PRO_SEKUNDE 25

bool flag_neue_metadaten = false;
unsigned int iDABtimeout = 0;

char buf[64];
char now[64];
char genre[64];
char bitrate[64];
char streamURL[255];
char statusPlaying[65];

char channelsscanned[64];
char stationsfound[64];

char channel[64];
char channel_s[64];
char channel_b[64];

int modus = 1;
// 0 = STBY
// 1 = MENUE
// 2 = IRADIO
// 3 = DAB
// 31 = DABscan

int modus_menu_selected = 1;
// 0 = DAB
// 1 = IRADIO


void rmSubstr(char *str, const char *toRemove)
{
	size_t length = strlen(toRemove);
	while((str = strstr(str, toRemove)))
	{
		memmove(str, str + length, 1 + strlen(str + length));
	}
}

bool isVLCplaying() {
        if (strstr(statusPlaying,"playing") != NULL)
                return 1;
        else
        return 0;
}

Uint32 vlc_Callback(Uint32 interval, void *param)
{
	//FILE *text =  popen("ls *", "r");
	FILE *text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r");
	FILE *text_now_playing = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep now_playing:", "r");
	FILE *text_genre = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep genre:", "r");
	FILE *text_bitrate = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep Bitrate:", "r");
	FILE *text_streamURL = popen("echo \"status\" | nc 127.0.0.1 9294 -N | grep \"new input:\" ", "r");
	FILE *text_statusPlaying = popen("echo \"status\" | nc 127.0.0.1 9294 -N | grep state", "r");

	// DABscan?
	if (modus == 31) {
	  FILE *text_channelsscanned =  popen("/home/pi/iRadio/Tuner/DABplus/dabscanstatus.sh channelsscanned", "r");
	  FILE *text_stationsfound =  popen("/home/pi/iRadio/Tuner/DABplus/dabscanstatus.sh stationsfound", "r");
	  
	  fgets(channelsscanned,50,text_channelsscanned);
	  fgets(stationsfound,50,text_stationsfound);
         
          rmSubstr(channelsscanned,"\r");
	  rmSubstr(stationsfound,"\r");

	  pclose(text_channelsscanned);
	  pclose(text_stationsfound);

	  //cout << "Channels scanned:" << channelsscanned;
	  //cout << "Stations found:" << stationsfound;
	} // if (modus == 31) {

	strcpy(buf,"empty ");
        strcpy(now,"... ");
        strcpy(bitrate,"... ");
        strcpy(streamURL,"... ");
        strcpy(statusPlaying,"... ");

	fgets(buf, 50, text);
	fgets(now, 60, text_now_playing);
	fgets(genre, 40, text_genre);
	fgets(bitrate, 40, text_bitrate);
	fgets(streamURL, 255, text_streamURL);
	fgets(statusPlaying,64, text_statusPlaying);

	rmSubstr(buf,"| title:");
	rmSubstr(buf,"\r");

         // in DAB-Modus
        if (modus==3) {
          if (strstr(buf,"DAB")==NULL) {
               strcpy(buf,"connecting ... \0");
            }
          if (isVLCplaying())
                 iDABtimeout=0;

          if (!isVLCplaying()) {
              strcpy(buf,"connecting ...  \0");
              iDABtimeout++;
              if (iDABtimeout>10)
                 strcpy(buf,"no audio service or data service \0");
          }
        }
	strcpy(channel,buf);

	rmSubstr(now,"| now_playing:");
	rmSubstr(now,"\r");
	strcpy(channel_s,now);

	rmSubstr(genre,"| genre:");
	rmSubstr(genre,"\r");

	rmSubstr(bitrate,"| Bitrate:");
	rmSubstr(bitrate,"\r");
	strcpy(channel_b,bitrate);

	rmSubstr(streamURL,"> ( new input: ");
	rmSubstr(streamURL," )");

	rmSubstr(statusPlaying,"( state ");
	rmSubstr(statusPlaying," )");
	rmSubstr(statusPlaying," ");

	pclose(text);
	pclose(text_now_playing);
	pclose(text_genre);
	pclose(text_bitrate);
	pclose(text_streamURL);
	pclose(text_statusPlaying);

	flag_neue_metadaten = true;
	return(interval);
}

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
			system("echo \"dab0\" | nc -u 127.0.0.1 9914 -w 0");
			system("killall vlc");
			modus=0;
		}

		if (strcmp((const char *)buffer,(const char *) "menu") == 0 ) {
			fprintf (stderr,"Gehe in Menue.\n");
   	                system("echo \"dab0\" | nc -u 127.0.0.1 9914 -w 0");
	                system("killall vlc");
			system("killall dabscan.sh");
			modus=1;
		}

                if (strcmp((const char *)buffer,(const char *) "entr") == 0 ) {
                        fprintf (stderr,"Enter.\n");
			// in Menu ?
			if (modus==1) {
                            if (modus_menu_selected==0)
				system("echo \"dabd\" | nc -u 127.0.0.1 6030 -w 0");
                            if (modus_menu_selected==1)
				system("echo \"inet\" | nc -u 127.0.0.1 6030 -w 0");
			}
                }

		if (strcmp((const char *)buffer,(const char *) "inet") == 0 ) {
			fprintf (stderr,"Gehe in Internetradiomodus.\n");
			system("vlcd &");
			modus=2;
		}

                if (strcmp((const char *)buffer,(const char *) "dabd") == 0 ) {
                        fprintf (stderr,"Gehe in DAB-Radiomodus.\n");
                        system("killall dabd");
			system("killall dabscan.sh");
			system("fuser -k -n udp 9914");
			system("/home/pi/iRadio/Tuner/DABplus/dabd &");
			SDL_Delay(3000);
			system("echo \"dab1\" | nc -u 127.0.0.1 9914 -w 0");
                        modus=3;
                }

                if (strcmp((const char *)buffer,(const char *) "next") == 0 ) {
                        fprintf (stderr,"next\n");
			if (modus==1) {
                          if (modus_menu_selected==0)
				modus_menu_selected=1;
			  else
				modus_menu_selected=0;
			}
		        if (modus==2)
  			   system("echo \"next\" | nc 127.0.0.1 9294 -N");
			if (modus==3) {
                            system("echo \"next\" | nc -u 127.0.0.1 9914 -w 0");
                            iDABtimeout=0;
			}
                }

                if (strcmp((const char *)buffer,(const char *) "prev") == 0 ) {
                        fprintf (stderr,"prev\n");
                        if (modus==1) {
                          if (modus_menu_selected==0)
                                modus_menu_selected=1;
                          else
                                modus_menu_selected=0;
                        }
                        if (modus==2)
                           system("echo \"prev\" | nc 127.0.0.1 9294 -N");
                        if (modus==3) {
                            system("echo \"prev\" | nc -u 127.0.0.1 9914 -w 0");
			    iDABtimeout=0;
			}
                }

                if (strcmp((const char *)buffer,(const char *) "dabs") == 0 ) {
			if (modus==3) {
                        	fprintf (stderr,"Gehe in DAB-scanmodus.\n");
				system("echo \"dab0\" | nc -u 127.0.0.1 9914 -w 0");
				system("killall vlc");
				system("killall dabd");
				system("killall dabscan.sh");
				system("fuser -k -n udp 9914");
			        system("/home/pi/iRadio/Tuner/DABplus/dabscan.sh &");
                        	modus=31;
			}
                }

	}

	close(sockfd);
	return 0;
}

int main(void) {
// close open port
	system("fuser -k -n udp 6030");
	time_t now;

// Interprozesskom herstellen
	pthread_t process;
	pthread_create(&process, NULL, socket_thread_modiswitch, NULL);
	pthread_detach(process);

// SDL - Teil
	SDL_Event event;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *textur_DAB = NULL;
	SDL_Texture *textur_IRADIO = NULL;
        SDL_Texture *textur_SELECTEDICON = NULL;


// Position und Dimension der Icons im Menu
	SDL_Rect IconDABRect;
	IconDABRect.x = 50; IconDABRect.y = 80;
	IconDABRect.w = 160;  IconDABRect.h = 160;

        SDL_Rect IconDABFrame = IconDABRect;
        IconDABFrame.x-=5; IconDABFrame.y-=5;
        IconDABFrame.w+=10; IconDABFrame.h+=10;

	SDL_Rect IconIRADIORect;
	IconIRADIORect.x = 270; IconIRADIORect.y = 80;
	IconIRADIORect.w = 160;  IconIRADIORect.h = 160;

        SDL_Rect IconIRADIOFrame = IconIRADIORect;
        IconIRADIOFrame.x-=5; IconIRADIOFrame.y-=5;
        IconIRADIOFrame.w+=10; IconIRADIOFrame.h+=10;

	SDL_Rect Line{0,40,SKALE_BREITE,2};
        SDL_Rect Line2{0,280,SKALE_BREITE,2};


	SDL_Rect IconModusRect;
        IconModusRect.x = 20; IconModusRect.y =110;
        IconModusRect.w = 80;  IconModusRect.h = 80;

	SDL_Rect DABscanRect;
        DABscanRect.x=120;DABscanRect.y=150;
	DABscanRect.w=38*8;DABscanRect.h=20;

	SDL_Rect DABscanfillRect;
        DABscanfillRect.x=120;DABscanfillRect.y=150;
	DABscanfillRect.w=1;  DABscanfillRect.h=20;

	SDL_Window *window = NULL;

	TTF_Font * font = NULL;
	TTF_Font * font2 = NULL;
	TTF_Font * font3 = NULL;

	SDL_Surface * surface_prgtitel = NULL;
	SDL_Texture * textur_prgtitel = NULL;

	SDL_Surface * surface_songtitel = NULL;
	SDL_Texture * textur_songtitel = NULL;

	SDL_Surface * surface_bitrate = NULL;
	SDL_Texture * textur_bitrate = NULL;

        SDL_Surface * surface_clock = NULL;
        SDL_Texture * textur_clock = NULL;

//	SDL_Surface * surface_modi = NULL;
//	SDL_Texture * textur_modi = NULL;

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Konnte SDL nicht initialisieren: %s", SDL_GetError());
		return 3;
	}

	if (SDL_CreateWindowAndRenderer(SKALE_BREITE, SKALE_HOEHE,  0, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window und Renderer konnten nicht erzeugt werden: %s", SDL_GetError());
		return 3;
	}

// load support for the PNG image formats
	int flags=IMG_INIT_PNG;
	if(IMG_Init(flags)&flags != flags) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PNG Unterstuetzung konnte nicht initialisiert werden: %s", IMG_GetError());
	return 3;
	}

	textur_DAB = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/DABgui/dab.png");

	if (textur_DAB == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Texturen: %s", IMG_GetError());
		return 3;
	}

        textur_IRADIO = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/DABgui/iRadio.png");

        if (textur_IRADIO == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Texturen: %s", IMG_GetError());
                return 3;
        }


        textur_SELECTEDICON = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/DABgui/select.png");

        if (textur_SELECTEDICON == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Texturen: %s", IMG_GetError());
                return 3;
        }


	if(TTF_Init()==-1) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_Init: %s", TTF_GetError());
		return 3;
	}

	font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 20);
	if(!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_OpenFont: %s", TTF_GetError());
		return 3;
	}

	font2 = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 36);
	if(!font2) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_OpenFont: %s", TTF_GetError());
		return 3;
	}

	font3 = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16);
	if(!font3) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_OpenFont: %s", TTF_GetError());
		return 3;
	}

	SDL_Color color = { 0, 0, 0 };
	SDL_Color color_white = { 255, 255, 255 };
	SDL_Color color_blue = { 72,61,139 };

// common_fps_init();

// Timer fuer cvlc Socketverbindung
	SDL_AddTimer(1000, vlc_Callback, NULL);

	//SDL_AddTimer(1000, vlc_DAB_Callback, NULL);

// Event-Schleife & Daemonbetrieb
	while (1) {
// current date/time based on current system
		time_t now = time(0);
	        tm *ltm = localtime(&now);


	// Programminformationen als Texturen vorbereiten
	// vermeiden von Nulllaengenfehler
		if (flag_neue_metadaten) {
			flag_neue_metadaten = false;
			channel[strlen(channel)-1]='\0';
			channel_s[strlen(channel_s)-1]='\0';
			channel_b[strlen(channel_b)-1]='\0';

			channelsscanned[strlen(channelsscanned)-1]='\0';
		        stationsfound[strlen(stationsfound)-1]='\0';

			// DABscan?
			if (modus == 31) {
			  strcpy(channel,"Sendersuche...");
 			  strcpy(channel_s,channelsscanned);
			  strcpy(channel_b,stationsfound);
			}
		}

		if (strlen(channel)!=0) {
			surface_prgtitel = TTF_RenderText_Solid(font, channel, color_white);
			if(!surface_prgtitel) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}
			textur_prgtitel = SDL_CreateTextureFromSurface(renderer,surface_prgtitel);
			if (textur_prgtitel == NULL) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in CreateTextureFromSurface: %s", SDL_GetError());
				return 3;
			}
		}
		if (strlen(channel_s)!=0) {
			surface_songtitel = TTF_RenderText_Solid(font3, channel_s, color_white);
			if(!surface_songtitel) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}

			textur_songtitel = SDL_CreateTextureFromSurface(renderer,surface_songtitel);
			if (textur_songtitel == NULL) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in CreateTextureFromSurface: %s", SDL_GetError());
				return 3;
			}
		}

		if (strlen(channel_b)!=0) {
			surface_bitrate = TTF_RenderText_Solid(font3, channel_b, color_white);
			if(!surface_bitrate) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}

			textur_bitrate = SDL_CreateTextureFromSurface(renderer,surface_bitrate);
			if (textur_bitrate == NULL) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in CreateTextureFromSurface: %s", SDL_GetError());
				return 3;
			}
		}

 		// Clock
		string clock_str, zero_hour, zero_min;
                if ((std::to_string(ltm->tm_min)).length() == 1)
                    zero_min = "0";

                if ((std::to_string(ltm->tm_hour)).length() == 1)
                    zero_hour = "0";

		clock_str = zero_hour + std::to_string(ltm->tm_hour) + ":" + zero_min + std::to_string(ltm->tm_min);

                surface_clock = TTF_RenderText_Solid(font2, clock_str.c_str(), color_white);
                if(!surface_clock) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
                    return 3;
                }
                textur_clock = SDL_CreateTextureFromSurface(renderer,surface_clock);
                if (textur_clock == NULL) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in CreateTextureFromSurface: %s", SDL_GetError());
                    return 3;
                }


// Position und Dimension Uhr
                int texW = 0; int texH = 0;
                SDL_QueryTexture(textur_clock, NULL, NULL, &texW, &texH);
                SDL_Rect dstrect_clock = { SKALE_BREITE/2-50, 2, texW, texH };

// Position und Dimension Programmanzeige
		SDL_QueryTexture(textur_prgtitel, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_p = { 120, 110, texW, texH };

// Position und Dimension Songanzeige
		SDL_QueryTexture(textur_songtitel, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_s = { 120, 150, texW, texH };

// Position und Dimension Bitratenanzeige
		SDL_QueryTexture(textur_bitrate, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_b = { 120, 180, texW, texH };





// Zeichne Skalenhintergrund ... oder Texture fuer Hintergrundbeleuchtung
                SDL_SetRenderDrawColor(renderer,48,48, 48, 255);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer,255,255, 255, 255);
                SDL_RenderFillRect(renderer,&Line);
                SDL_RenderFillRect(renderer,&Line2);


// zeichen Uhr in allen Modis
                SDL_RenderCopy(renderer, textur_clock, NULL, &dstrect_clock);

// Zeichne das Fenster des jeweils aktuellen Modus

                if (modus == 0) {  // STBY
                   SDL_SetRenderDrawColor(renderer,0,0,0, 255);
                   SDL_RenderClear(renderer);
                } // if (modus == 0)

	
                if (modus == 1) { // MENU
		   // welches ICON ist selektiert?
                   if (modus_menu_selected==0)
                      SDL_RenderCopyEx(renderer, textur_SELECTEDICON, NULL, &IconDABFrame, 0, NULL, SDL_FLIP_NONE);
                   else
                      SDL_RenderCopyEx(renderer, textur_SELECTEDICON, NULL, &IconIRADIOFrame, 0, NULL, SDL_FLIP_NONE);


                SDL_RenderCopyEx(renderer, textur_DAB, NULL, &IconDABRect, 0, NULL, SDL_FLIP_NONE);
                SDL_RenderCopyEx(renderer, textur_IRADIO, NULL, &IconIRADIORect, 0, NULL, SDL_FLIP_NONE);
               } // if (modus == 1)

//********************************************************************************************************************
               if (modus == 2) { // IRADIO
                 SDL_RenderCopyEx(renderer, textur_IRADIO, NULL, &IconModusRect, 0, NULL, SDL_FLIP_NONE);
	         // zeichne Sendernamen
                 SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect_p);
		 // zeichne Songtitel
                 SDL_RenderCopy(renderer, textur_songtitel, NULL, &dstrect_s);
		 // zeichne Bitrate
		 SDL_RenderCopy(renderer, textur_bitrate, NULL, &dstrect_b);

               } // if (modus == 2)

//********************************************************************************************************************

              if (modus == 3) { // DAB
                 SDL_RenderCopyEx(renderer, textur_DAB, NULL, &IconModusRect, 0, NULL, SDL_FLIP_NONE);
                 // zeichne Sendernamen
                 SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect_p);
              } // if (modus == 3) {

//********************************************************************************************************************

              if (modus == 31) { // DAB scan
                 SDL_RenderCopyEx(renderer, textur_DAB, NULL, &IconModusRect, 0, NULL, SDL_FLIP_NONE);
	         // zeichne Label "Sendersuchlauf ..."
                 SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect_p);
		 // zeichne Fortschrittsbalken
		 SDL_RenderDrawRect(renderer,&DABscanRect);
                 if (strlen(channelsscanned)!=0)
                    DABscanfillRect.w=8*stoi(channelsscanned);
                 SDL_RenderFillRect(renderer,&DABscanfillRect);
	
		 // Wenn bei Kanal 13F, starte dabd mit neuen Senderlisten und gehe in DAB-Radiomodus 
		 if (strstr(channelsscanned,"38")!=NULL) {
		    SDL_Delay(3000);
		    system("/home/pi/iRadio/Tuner/DABplus/dabd &");
                    SDL_Delay(1000);
		    system("echo \"dab1\" | nc -u 127.0.0.1 9914 -w 0");	    
	            modus=3;		
		 }
		 
                 
		 // zeichne Anzahl gefundener Sender
		 SDL_RenderCopy(renderer, textur_bitrate, NULL, &dstrect_b);
              } // if (modus == 31) {

//********************************************************************************************************************



              // Aktualisiere Fenster
	      SDL_RenderPresent(renderer);

	      // Eventbehandlung
	      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
		break;
	      // Limit max. xxx fps
	      SDL_Delay(1000/BILDER_PRO_SEKUNDE);

	      // Aufraeumarbeiten pro Frame
	      if (surface_prgtitel)
	  	SDL_FreeSurface(surface_prgtitel);
	      if (textur_prgtitel)
 		SDL_DestroyTexture(textur_prgtitel);
	      if (surface_songtitel)
		SDL_FreeSurface(surface_songtitel);
	      if (textur_songtitel)
	 	SDL_DestroyTexture(textur_songtitel);
	      if (surface_bitrate)
	 	SDL_FreeSurface(surface_bitrate);
	      if (textur_bitrate)
	 	SDL_DestroyTexture(textur_bitrate);
              if (surface_clock)
                SDL_FreeSurface(surface_clock);
              if (textur_clock)
                SDL_DestroyTexture(textur_clock);
	} // while(1)

// Aufraeumen und Ressourcen freigeben
	if (textur_DAB)
		SDL_DestroyTexture(textur_DAB);
	if (textur_IRADIO)
		SDL_DestroyTexture(textur_IRADIO);
        if (textur_SELECTEDICON)
                SDL_DestroyTexture(textur_SELECTEDICON);


	if (textur_prgtitel)
		SDL_DestroyTexture(textur_prgtitel);
	if (surface_prgtitel)
		SDL_FreeSurface(surface_prgtitel);
	if (textur_songtitel)
		SDL_DestroyTexture(textur_songtitel);
	if (surface_songtitel)
		SDL_FreeSurface(surface_songtitel);
	if (textur_bitrate)
		SDL_DestroyTexture(textur_bitrate);
        if (surface_bitrate)
                SDL_FreeSurface(surface_bitrate);
        if (textur_clock)
                SDL_DestroyTexture(textur_clock);
        if (surface_clock)
                SDL_FreeSurface(surface_clock);


	if (font)
		TTF_CloseFont(font);
	if (font2)
		TTF_CloseFont(font2);
	if (font3)
		TTF_CloseFont(font3);

	IMG_Quit();
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}
