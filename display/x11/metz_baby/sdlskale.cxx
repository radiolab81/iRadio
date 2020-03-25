#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define PLAYLIST "/home/pi/.config/vlc/playlist.m3u"
//#define PLAYLIST "playlist.m3u"

#define ZEIGERANSCHLAG_LINKS 120
#define ZEIGERANSCHLAG_RECHTS 440

#define SKALE_HOEHE 290
#define SKALE_BREITE 480

#define BILDER_PRO_SEKUNDE 20

#define PAUSE_BEIM_SENDERWECHSEL true	// true oder false

#define ANZAHL_SENDER (Senderliste.size()-1)

#define SENDERABSTAND (ZEIGERANSCHLAG_RECHTS-ZEIGERANSCHLAG_LINKS)/ANZAHL_SENDER

bool flag_neue_metadaten = false;

char buf[64];
char now[64];
char genre[64];
char bitrate[64];
char streamURL[255];

char channel[64];

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

    fgets(buf, 64, text);
    fgets(now, 64, text_now_playing);
    fgets(genre, 64, text_genre);
    fgets(bitrate, 64, text_bitrate);
    fgets(streamURL, 255, text_streamURL);

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

    pclose(text);
    pclose(text_now_playing);
    pclose(text_genre);
    pclose(text_bitrate);
    pclose(text_streamURL);

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

int main(void) {

    char SenderURL[255];
    vector<string> Senderliste;
    int i_alt, i;

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


    // SDL - Teil
    SDL_Event event;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *textur_skale = NULL;
    SDL_Texture *textur_zeiger = NULL;
    SDL_Window *window = NULL;

    TTF_Font * font = NULL;
    SDL_Surface * surface_prgtitel = NULL;
    SDL_Texture * textur_prgtitel = NULL;

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

    if ((textur_skale == NULL) || (textur_zeiger == NULL) ) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Skalen und Zeiger Texturen: %s", IMG_GetError());
	return 3;
    }

   if(TTF_Init()==-1) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_Init: %s", TTF_GetError());
    	return 3;
   }

   font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16);
   if(!font) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler in TTF_OpenFont: %s", TTF_GetError());
	return 3;
   }

   SDL_Color color_yellow = { 255, 0, 0 };
   //  printf("%s","Test\n");
   // common_fps_init();

   // Position und Dimension des Skalenzeigers
   SDL_Rect ZeigerRect;
   ZeigerRect.x = 150; ZeigerRect.y = 0;
   ZeigerRect.w = 10;	ZeigerRect.h = 377;

   // Timer fuer cvlc Socketverbindung
   SDL_AddTimer(1000, vlc_Callback, NULL);

   // Event-Schleife & Daemonbetrieb
   while (1) {
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
    	if (ZeigerRect.x > (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS ) {
			if (PAUSE_BEIM_SENDERWECHSEL)
   				system("echo \"stop\" | nc 127.0.0.1 9294 -N");

        	ZeigerRect.x--;
	
			if (PAUSE_BEIM_SENDERWECHSEL) 
  			 if (ZeigerRect.x == (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS) 
   					system("echo \"play\" | nc 127.0.0.1 9294 -N");			
    	}

    	if (ZeigerRect.x < (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS ) {
			if (PAUSE_BEIM_SENDERWECHSEL)
   				system("echo \"stop\" | nc 127.0.0.1 9294 -N");

       		ZeigerRect.x++;

			if (PAUSE_BEIM_SENDERWECHSEL) 
  			 if (ZeigerRect.x == (SENDERABSTAND*i) + ZEIGERANSCHLAG_LINKS) 
   					system("echo \"play\" | nc 127.0.0.1 9294 -N");		
    	}

    	// Zeichne Skalenglase
    	SDL_RenderCopy(renderer, textur_skale, NULL, NULL);

    	// Zeichne Zeiger
    	SDL_RenderCopy(renderer, textur_zeiger, NULL, &ZeigerRect);

		// Zeichen Programmnamen auf Skalenglas
		// vermeiden von Nulllaengenfehler
		if (flag_neue_metadaten) {
			flag_neue_metadaten = false;
			channel[strlen(channel)-1]='\0';
        }

		if (strlen(channel)!=0) {
 			surface_prgtitel = TTF_RenderText_Solid(font, channel, color_yellow);
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

    	int texW = 0; int texH = 0;
  		SDL_QueryTexture(textur_prgtitel, NULL, NULL, &texW, &texH);
    	SDL_Rect dstrect = { 20, 270, texW, texH };

    	SDL_RenderCopy(renderer, textur_prgtitel, NULL, &dstrect);

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

    } // while(1)

    // Aufraeumen und Ressourcen freigeben
    if (textur_skale)
    	SDL_DestroyTexture(textur_skale);

    if (textur_zeiger)
	SDL_DestroyTexture(textur_zeiger);

    if (textur_prgtitel)
	SDL_DestroyTexture(textur_prgtitel);

    if (surface_prgtitel)
	SDL_FreeSurface(surface_prgtitel);

    if (font)
	TTF_CloseFont(font);

    IMG_Quit();

    if (renderer)
	SDL_DestroyRenderer(renderer);

    if (window)
	SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
