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

#define PLAYLIST "/home/pi/.config/vlc/playlist.m3u"

#define SKALE_HOEHE 480
#define SKALE_BREITE 800

#define BILDER_PRO_SEKUNDE 25

bool flag_neue_metadaten = false;

char buf[64];
char now[64];
char genre[64];
char bitrate[64];
char streamURL[255];
char statusPlaying[65];

char channel[64];
char channel_s[64];
char channel_b[64];

int modus = 0;
// 0 = Internetradio
// 1 = Standby
// 2 = Bluetooth

void rmSubstr(char *str, const char *toRemove)
{
	size_t length = strlen(toRemove);
	while((str = strstr(str, toRemove)))
	{
		memmove(str, str + length, 1 + strlen(str + length));
	}
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

	fgets(buf, 30, text);
	fgets(now, 60, text_now_playing);
	fgets(genre, 40, text_genre);
	fgets(bitrate, 40, text_bitrate);
	fgets(streamURL, 255, text_streamURL);
	fgets(statusPlaying,64, text_statusPlaying);

	rmSubstr(buf,"| title:");
	rmSubstr(buf,"\r");
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

bool isVLCplaying() {
	if (strstr(statusPlaying,"playing") != NULL)
		return 1;
	else
	return 0;
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
			modus=1;
			system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
		}

		if (strcmp((const char *)buffer,(const char *) "netr") == 0 ) {
			fprintf (stderr,"Gehe in Internetradiomodus.\n");
			modus=0;
			system("echo \"play\" | nc 127.0.0.1 9294 -w 0");
		}

		if (strcmp((const char *)buffer,(const char *) "blth") == 0 ) {
			fprintf (stderr,"Gehe in Bluetoothmodus.\n");
			modus=2;
			system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
		}
	}

	close(sockfd);
	return 0;
}

int main(void) {

	time_t now;

// Interprozesskom herstellen
	pthread_t process;
	pthread_create(&process, NULL, socket_thread_modiswitch, NULL);
	pthread_detach(process);

// SDL - Teil
	SDL_Event event;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *textur_cassette = NULL;
	SDL_Texture *textur_cassette_stby = NULL;
	SDL_Texture *textur_spule = NULL;
	SDL_Texture *textur_bluetooth =  NULL; 

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

	textur_cassette = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/cassettensim/cassette.png");

	if (textur_cassette == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Skalen und Zeiger Texturen: %s", IMG_GetError());
		return 3;
	}

        textur_cassette_stby = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/cassettensim/cassette_off.png");

        if (textur_cassette_stby == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Skalen und Zeiger Texturen: %s", IMG_GetError());
                return 3;
        }

        textur_spule = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/cassettensim/spule.png");

        if (textur_spule == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Skalen und Zeiger Texturen: %s", IMG_GetError());
                return 3;
        }

        textur_bluetooth = IMG_LoadTexture(renderer, "/home/pi/iRadio/display/x11/cassettensim/bluetooth.png");

        if (textur_bluetooth == NULL) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Skalen und Zeiger Texturen: %s", IMG_GetError());
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

// Position und Dimension der Kassetenspulen
	SDL_Rect SpuleLinksRect;
   	SpuleLinksRect.x = 191; SpuleLinksRect.y = 175;
   	SpuleLinksRect.w = 91;  SpuleLinksRect.h = 90;

        SDL_Rect SpuleRechtsRect;
        SpuleRechtsRect.x = 520; SpuleRechtsRect.y = 175;
        SpuleRechtsRect.w = 91;  SpuleRechtsRect.h = 90;

	int rotationswinkel = 0;

// Timer fuer cvlc Socketverbindung
	SDL_AddTimer(1000, vlc_Callback, NULL);

// Event-Schleife & Daemonbetrieb
	while (1) {
// current date/time based on current system
		time_t now = time(0);

// Zeichen Skalenhintergrund ... oder Texture fuer Hintergrundbeleuchtung
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                SDL_RenderClear(renderer);


	  if (modus == 0) {  // Internetradiomodus
// Rotationsgeschwindigkeit der Kassettenspulen
		rotationswinkel += 2;
		if ( rotationswinkel > 360)
		  rotationswinkel = 0;

// Zeichne Skalenglas
		SDL_RenderCopyEx(renderer, textur_spule, NULL, &SpuleLinksRect, rotationswinkel, NULL, SDL_FLIP_NONE);
                SDL_RenderCopyEx(renderer, textur_spule, NULL, &SpuleRechtsRect, rotationswinkel, NULL, SDL_FLIP_NONE);
		SDL_RenderCopy(renderer, textur_cassette, NULL, NULL);

          } // if (modus == 0)

	  if (modus == 1) { // Standbymodus
		// Rotationsgeschwindigkeit der Kassettenspulen
                rotationswinkel = 0;

// Zeichne Skalenglas
                SDL_RenderCopy(renderer, textur_cassette_stby, NULL, NULL);

	 } // if (modus == 1)

          if (modus == 2) { // Bluetoothmodus

// Zeichne Skalenglas
	        SDL_SetRenderDrawColor(renderer,255,255,255,255);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, textur_bluetooth, NULL, NULL);

         } // if (modus == 2)


// Zeichen der Programminformationen auf Skalenglas
// vermeiden von Nulllaengenfehler
		if (flag_neue_metadaten) {
			flag_neue_metadaten = false;
			channel[strlen(channel)-1]='\0';
			channel_s[strlen(channel_s)-1]='\0';
			channel_b[strlen(channel_b)-1]='\0';
		}

		if (strlen(channel)!=0) {
 			surface_prgtitel = TTF_RenderText_Solid(font2, channel, color);
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
			surface_songtitel = TTF_RenderText_Solid(font3, channel_s, color);
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
			surface_bitrate = TTF_RenderText_Solid(font3, channel_b, color);
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

// Position und Dimension Programmanzeige
		int texW = 0; int texH = 0;
		SDL_QueryTexture(textur_prgtitel, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_p = { 400, 59, texW, texH };

// Position und Dimension Songanzeige
		SDL_QueryTexture(textur_songtitel, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_s = { 407, 104, texW, texH };

// Position und Dimension Bitratenanzeige
		SDL_QueryTexture(textur_bitrate, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect_b = { 407, 130, texW, texH };


	if (modus == 0 ) { // Internetradio
// zeichne Sendernamen
		SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect_p);
// zeichne Songtitel
		SDL_RenderCopy(renderer, textur_songtitel, NULL, &dstrect_s);
// zeichne Bitrate
		SDL_RenderCopy(renderer, textur_bitrate, NULL, &dstrect_b);
	} // if (modus == 0 )

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
	} // while(1)

// Aufraeumen und Ressourcen freigeben
	if (textur_cassette)
		SDL_DestroyTexture(textur_cassette);
	if (textur_cassette_stby)
		SDL_DestroyTexture(textur_cassette_stby);
	if (textur_spule)
		SDL_DestroyTexture(textur_spule);
	if (textur_bluetooth)
		SDL_DestroyTexture(textur_bluetooth);
	if (textur_prgtitel)
		SDL_DestroyTexture(textur_prgtitel);
	if (surface_prgtitel)
		SDL_FreeSurface(surface_prgtitel);
	if (textur_songtitel)
		SDL_DestroyTexture(textur_songtitel);
	if (surface_songtitel)
		SDL_FreeSurface(surface_songtitel);
	if (textur_bitrate)
		SDL_FreeSurface(surface_bitrate);
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
