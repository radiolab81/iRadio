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
//#define PLAYLIST "playlist.m3u"

#define ZEIGERANSCHLAG_LINKS 0
#define ZEIGERANSCHLAG_RECHTS 180

#define SKALE_HOEHE 480
#define SKALE_BREITE 800

#define BILDER_PRO_SEKUNDE 50

#define PAUSE_BEIM_SENDERWECHSEL true	// true oder false

#define ANZAHL_SENDER (Senderliste.size()-1)

#define SENDERABSTAND (ZEIGERANSCHLAG_RECHTS-ZEIGERANSCHLAG_LINKS)/ANZAHL_SENDER

bool flag_neue_metadaten = false;

char buf[64];
char now[64];
char genre[64];
char bitrate[64];
char streamURL[255];
char statusPlaying[65];

char channel[64];

int modus = 0;
// 0 = Internetradio
// 1 = Standby
// 2 = Bluetooth


struct Weckzeit{ // The object to be serialized / deserialized
public:
  	int stunde;
  	int minute;
};

Weckzeit wecker;

void write(const std::string& file_name, Weckzeit& data) // Writes the given OBJECT data to the given file name.
{
        std::ofstream out;
        out.open(file_name,std::ios::binary);
        out.write(reinterpret_cast<char*>(&data), sizeof(Weckzeit));
        out.close();
};

void read(const std::string& file_name, Weckzeit& data) // Reads the given file and assigns the data to the given OBJECT.
{
        std::ifstream in;
        in.open(file_name,std::ios::binary);
        in.read(reinterpret_cast<char*>(&data), sizeof(Weckzeit));
        in.close();
};


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

    fgets(buf, 64, text);
    fgets(now, 64, text_now_playing);
    fgets(genre, 64, text_genre);
    fgets(bitrate, 64, text_bitrate);
    fgets(streamURL, 255, text_streamURL);
	fgets(statusPlaying,64, text_statusPlaying);

    rmSubstr(buf,"| title:");
    rmSubstr(buf,"\r");
	strcpy(channel,buf);

    rmSubstr(now,"| now_playing:");
    rmSubstr(now,"\r");

    rmSubstr(genre,"| genre:");
    rmSubstr(genre,"\r");

    rmSubstr(bitrate,"| Bitrate:");
    rmSubstr(bitrate,"\r");

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


int aktueller_Sender(vector<string> Liste, string URL) {
   //cout << "URL ist:" <<  URL << " size: "  << URL.size() << "\n";
   for (int i=0; i<Liste.size(); i++) {
		if (URL.rfind(Liste.at(i)) != -1)
	     	return i;
   }

  return -1;
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

                if (strcmp((const char *)buffer,(const char *) "weck") == 0 ) {
                        // Weckzeit einlesen
			read("/home/pi/iRadio/weckzeit.txt",wecker);
			cout << "neue Weckzeit: " << wecker.stunde << ":" << wecker.minute << "\n";
                }

 	}

	close(sockfd);
	return 0;
}




int main(void) {

    char SenderURL[255];
    vector<string> Senderliste;
    int i_alt = 0, i = 0;

    time_t now;

	ifstream in(PLAYLIST);

	if(!in) {
 		cout << "Senderliste nicht gefunden/kann nicht geoeffnet werden.\n";
	    	return 1;
  	}

	while(in) {
	    	in.getline(SenderURL, 255);  // delim defaults to '\n'
    		if(in) {
            		string str = SenderURL;
      			Senderliste.push_back(str);
     		}
  	}

	in.close();

    cout << "Senderliste enthaelt " << Senderliste.size() << " Sender.\n";

    // Weckzeit einlesen
    read("/home/pi/iRadio/weckzeit.txt",wecker);

    // Interprozesskom herstellen
    pthread_t process;
    pthread_create(&process, NULL, socket_thread_modiswitch, NULL);
	pthread_detach(process);
 
    // SDL - Teil
    SDL_Event event;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *textur_skale = NULL;
    SDL_Texture *textur_zeiger = NULL;

    SDL_Texture *textur_zeiger_stunde = NULL;
    SDL_Texture *textur_zeiger_minute = NULL;

    SDL_Texture *textur_zeiger_wecker = NULL;

    SDL_Window *window = NULL;

    TTF_Font * font = NULL;
    TTF_Font * font2 = NULL;

    SDL_Surface * surface_prgtitel = NULL;
    SDL_Texture * textur_prgtitel = NULL;

    SDL_Surface * surface_modi = NULL;
    SDL_Texture * textur_modi = NULL;

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

    textur_skale = IMG_LoadTexture(renderer, "skale.png");
    textur_zeiger = IMG_LoadTexture(renderer, "zeiger.png");
    textur_zeiger_stunde = IMG_LoadTexture(renderer, "zeiger_stunde.png");
    textur_zeiger_minute = IMG_LoadTexture(renderer, "zeiger_minute.png");

    textur_zeiger_wecker = IMG_LoadTexture(renderer, "zeiger_wecker.png");

    if ((textur_skale == NULL) || (textur_zeiger == NULL)  || (textur_zeiger_stunde == NULL)  || (textur_zeiger_minute == NULL)
		|| (textur_zeiger_wecker == NULL)) {
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

   SDL_Color color = { 255, 0, 0 };
   SDL_Color color_white = { 255, 255, 255 };
   //  printf("%s","Test\n");
   // common_fps_init();

   // Position und Dimension des Skalenzeigers
   SDL_Rect ZeigerRect;
   ZeigerRect.x = 240; ZeigerRect.y = 110;
   ZeigerRect.w = 320;	ZeigerRect.h = 320;
   int rotationswinkel=ZEIGERANSCHLAG_LINKS;

   // Timer fuer cvlc Socketverbindung
   SDL_AddTimer(1000, vlc_Callback, NULL);

   // Event-Schleife & Daemonbetrieb
   while (1) {
 	// current date/time based on current system
   	time_t now = time(0);

   	// welcher Sender wird von vlc gerade wiedergeben ?
    	string str = streamURL;

	i_alt = i;
    	i = aktueller_Sender(Senderliste,str);

	// URL wegen URL Redirection nicht in Senderliste? Dann belasse Zeiger an Stelle.
	if (i == -1)
		i = i_alt;

    	// Zeichen Skalenhintergrund ... oder Texture fuer Hintergrundbeleuchtung
    	SDL_SetRenderDrawColor(renderer,255,255,255,255);
    	SDL_RenderClear(renderer);


    	// Verschiebe Skalenzeigerposition zum eingestellten Sender
    	if (rotationswinkel > (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS ) {
			if (PAUSE_BEIM_SENDERWECHSEL)
   				system("echo \"stop\" | nc 127.0.0.1 9294 -N");

        	rotationswinkel--;

			if (PAUSE_BEIM_SENDERWECHSEL)
  			 if (rotationswinkel == (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS )
   					system("echo \"play\" | nc 127.0.0.1 9294 -N");

    	}

    	if (rotationswinkel < (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS ) {
			if (PAUSE_BEIM_SENDERWECHSEL)
   				system("echo \"stop\" | nc 127.0.0.1 9294 -N");

       		rotationswinkel++;

			if (PAUSE_BEIM_SENDERWECHSEL)
  			 if (rotationswinkel == (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS )
   					system("echo \"play\" | nc 127.0.0.1 9294 -N");
    	}


   		// Zeichne Skalenglas
   		SDL_RenderCopy(renderer, textur_skale, NULL, NULL);


	 	if (modus == 0) { // Internetradio
    		// Zeichne Skalenzeiger
    		SDL_RenderCopyEx(renderer, textur_zeiger, NULL, &ZeigerRect,rotationswinkel-45,NULL,SDL_FLIP_NONE);
		} // if (modus == 0 ) { // Internetradio


	 	if ((modus == 1) || (modus == 2)) { // Standby-Screen

			// Wecker an? existiert eine alarman.status
			ifstream in("/home/pi/.config/vlc/alarman.status");
			if(in) {
				in.close();
				// ja, dann:

				// Weckerzeiger zeichnen
				SDL_RenderCopyEx(renderer, textur_zeiger_wecker, NULL, &ZeigerRect,45+(30*wecker.stunde)+(0.5*wecker.minute),NULL,SDL_FLIP_NONE);
			}

			// Zeichne Uhr
			tm *ltm = localtime(&now);

           		if (ltm->tm_hour > 12)
				 ltm->tm_hour = ltm->tm_hour - 12;

			SDL_RenderCopyEx(renderer, textur_zeiger_minute, NULL, &ZeigerRect,45+(ltm->tm_hour*30)+(ltm->tm_min/2)+6,NULL,SDL_FLIP_NONE);
    			SDL_RenderCopyEx(renderer, textur_zeiger_stunde, NULL, &ZeigerRect,45+(ltm->tm_min*6)+6 ,NULL,SDL_FLIP_NONE);

		} // ...{ // Standby-Screen


		// Zeichen Programmnamen auf Skalenglas
		// vermeiden von Nulllaengenfehler
		if (flag_neue_metadaten) {
			flag_neue_metadaten = false;
			channel[strlen(channel)-1]='\0';
        	}

		if (strlen(channel)!=0) {
 			surface_prgtitel = TTF_RenderText_Solid(font, channel, color);
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


        // Position und Dimension Programmanzeige
    	int texW = 0; int texH = 0;
  		SDL_QueryTexture(textur_prgtitel, NULL, NULL, &texW, &texH);
    	SDL_Rect dstrect = { 120, 450, texW, texH };


		if (modus == 0 ) { // Internetradio
			// Zeichne Sendernamen
    		SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect);
		} // if (modus == 0 ) { // Internetradio



		// Zeichnen Label Betriebsmodus
	 	if (modus == 0) { // Internetradio
 			surface_modi = TTF_RenderText_Solid(font2, "NET", color_white);
			if(!surface_modi) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}
		}

		if (modus == 1) {
 			surface_modi = TTF_RenderText_Solid(font2, "STB", color_white);
			if(!surface_modi) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}
		}

		if (modus == 2) {
 			surface_modi = TTF_RenderText_Solid(font2, "BT", color_white);
			if(!surface_modi) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_RenderText_Solid: %s", TTF_GetError());
				return 3;
			}
		}

    	textur_modi = SDL_CreateTextureFromSurface(renderer,surface_modi);
		if (textur_modi == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in CreateTextureFromSurface: %s", SDL_GetError());
			return 3;
    	}


        // Position und Dimension Betriebsmodi
    	texW = 0; texH = 0;
  		SDL_QueryTexture(textur_modi, NULL, NULL, &texW, &texH);
    	SDL_Rect dstrect_modi = { 710, 360, texW, texH };

    	SDL_RenderCopy(renderer, textur_modi, NULL, &dstrect_modi);


    	// Aktualisiere Fenster
    	SDL_RenderPresent(renderer);
    	//  common_fps_update_and_print();

    	// Eventbehandlung
    	if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
		break;

		SDL_Delay(1000/BILDER_PRO_SEKUNDE); // Limit max. xxx fps

		// Aufraeumarbeiten pro Frame
		if (surface_prgtitel)
			SDL_FreeSurface(surface_prgtitel);

		if (textur_prgtitel)
 			SDL_DestroyTexture(textur_prgtitel);

   		if (textur_modi)
			SDL_DestroyTexture(textur_modi);

    	if (surface_modi)
			SDL_FreeSurface(surface_modi);

    } // while(1)

    // Aufraeumen und Ressourcen freigeben
    if (textur_skale)
    	SDL_DestroyTexture(textur_skale);

    if (textur_zeiger)
	SDL_DestroyTexture(textur_zeiger);

    if (textur_zeiger_stunde)
	SDL_DestroyTexture(textur_zeiger_stunde);

    if (textur_zeiger_minute)
	SDL_DestroyTexture(textur_zeiger_minute);

    if (textur_zeiger_wecker)
	SDL_DestroyTexture(textur_zeiger_wecker);

    if (textur_prgtitel)
	SDL_DestroyTexture(textur_prgtitel);

    if (surface_prgtitel)
	SDL_FreeSurface(surface_prgtitel);

    if (textur_modi)
	SDL_DestroyTexture(textur_modi);

    if (surface_modi)
	SDL_FreeSurface(surface_modi);

    if (font)
	TTF_CloseFont(font);

    if (font2)
	TTF_CloseFont(font2);

    IMG_Quit();

    if (renderer)
	SDL_DestroyRenderer(renderer);

    if (window)
	SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
