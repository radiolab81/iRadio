#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#define ROEHREN_HOEHE 400
#define ROEHREN_BREITE 100

#define BILDER_PRO_SEKUNDE 10

using namespace std;

bool flag_neue_metadaten = false;

char buf[64];
char bitrate[64];

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
    FILE *text = popen("./signal", "r");
    FILE *text_bitrate = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep Bitrate:", "r");

    fgets(buf, 64, text);
    fgets(bitrate, 64, text_bitrate);
 
    rmSubstr(buf,"\r");

    rmSubstr(bitrate,"| Bitrate:");
    rmSubstr(bitrate,"\r");

    pclose(text);
    pclose(text_bitrate);

    printf("MagicEyeD-Signalstaerke WiFi: %s",buf);

    flag_neue_metadaten = true;
    return(interval);
}

int main(void) {
    // SDL - Teil
    SDL_Event event;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *textur_tube = NULL;
    SDL_Texture *textur_tubemitte = NULL;
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Konnte SDL nicht initialisieren: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(ROEHREN_BREITE, ROEHREN_HOEHE,  0, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window und Renderer konnten nicht erzeugt werden: %s", SDL_GetError());
        return 3;
    }


    // load support for the PNG image formats
    int flags=IMG_INIT_PNG;
    if(IMG_Init(flags)&flags != flags) {
     	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PNG Unterstuetzung konnte nicht initialisiert werden: %s", IMG_GetError());
	return 3;
    }

    textur_tube = IMG_LoadTexture(renderer, "tube.png");
    textur_tubemitte = IMG_LoadTexture(renderer, "tube2.png");

    if (textur_tube == NULL) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fehler beim Laden der Texturen: %s", IMG_GetError());
	return 3;
    }

   // Position und Dimension der Roehrenmitte
   SDL_Rect DestR;
  // DestR.w = (int) (ROEHREN_BREITE/2.2);	DestR.h = (int) (ROEHREN_HOEHE/2.2);
   DestR.x = 20; //DestR.y = ROEHREN_HOEHE/2;

   // Timer fuer cvlc Socketverbindung
   SDL_AddTimer(1000, vlc_Callback, NULL);

   // Event-Schleife & Daemonbetrieb
   while (1) {
    	// Zeichen Skalenhintergrund ... oder Texture fuer Hintergrundbeleuchtung
    	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    	SDL_RenderClear(renderer);

    	// Zeichne das magische Auge
    	SDL_RenderCopy(renderer, textur_tube, NULL, NULL);
		// zeichne Faecher entsprechend der WiFi-Signalstärke (oder Bitrate des Audiostreams)
        DestR.w = ROEHREN_BREITE-38;	
	    DestR.y = (ROEHREN_HOEHE/2)-(((100-atoi(buf))/100.0)*(ROEHREN_HOEHE/2));
	    DestR.h = (ROEHREN_HOEHE)*((100-atoi(buf))/100.0);
	    SDL_RenderCopy(renderer, textur_tubemitte, NULL, &DestR);  

    	// Aktualisiere Fenster
    	SDL_RenderPresent(renderer);
   
    	// Eventbehandlung
    	if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
		  break;

	SDL_Delay(1000/BILDER_PRO_SEKUNDE); // Limit max. xxx fps

    } // while(1)

    // Aufraeumen und Ressourcen freigeben
    if (textur_tube)
    	SDL_DestroyTexture(textur_tube);

    if (textur_tubemitte)
	    SDL_DestroyTexture(textur_tubemitte);

    IMG_Quit();

    if (renderer)
	SDL_DestroyRenderer(renderer);

    if (window)
	SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}

