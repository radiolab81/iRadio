#include <string.h>
#include <iostream>

//#define DEBUG
#define DEFAULT_LANG "en-US"
#define GERMAN_LANG "de-DE"
#define ENGLISH_LANG "en-GB"
#define ENGLISH_LANG_US "en-US"
#define FRENCH_LANG "fr-FR"
#define SPANISH_LANG "es-ES"
#define ITAL_LANG "it-IT"

using namespace std;

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

int main()
{
    char url_from_vlc[2048];
    char url_to_remove[2048];
    char port_to_remove[6];

    FILE *text = popen("echo \"status\" | nc 127.0.0.1 9294 -N | grep input", "r");
    fgets(url_from_vlc, 2048, text);

    if (text != NULL) {
       pclose(text);
       text = NULL;
    }

    if (url_from_vlc[0]!='>') {
      printf(DEFAULT_LANG);
      exit(0);
    }

    // remove protocol (http, https, ...)
    rmSubstr(url_from_vlc,"> ( new input: http://");
    rmSubstr(url_from_vlc,"> ( new input: https://");
    rmSubstr(url_from_vlc,"\r");

    // isolate domain
    if (strchr(url_from_vlc,'/')!=NULL) {
      strcpy(url_to_remove,strchr(url_from_vlc,'/'));
      rmSubstr(url_from_vlc,url_to_remove);
    }

    // remove Portnumbers
    if (strchr(url_from_vlc,':')!=NULL) {
      strcpy(port_to_remove,strchr(url_from_vlc,':'));
      rmSubstr(url_from_vlc,port_to_remove);
    }

#ifdef DEBUG
    printf("Domain is: %s\n",url_from_vlc);
#endif

    // hier jetzt geoiplookup aufrufen
    string cmd;
    char result[2048];
    cmd = "geoiplookup ";
    cmd += url_from_vlc;

    text = popen(cmd.c_str(), "r");
    fgets(result, 2048, text);

    if (text != NULL) {
       pclose(text);
       text = NULL;
    }

#ifdef DEBUG
    printf("Domain ist from %s\n",result);
#endif

    // hier Auswertung welches Sprachpacket gewaehlt werden soll
    if (strstr(result,"Germany")!=NULL) {
      printf(GERMAN_LANG); exit(0); }

    if (strstr(result,"Austria")!=NULL) {
      printf(GERMAN_LANG); exit(0); }

    if (strstr(result,"Switzerland")!=NULL) {
      printf(GERMAN_LANG); exit(0); }

    if (strstr(result,"United States")!=NULL) {
      printf(ENGLISH_LANG_US); exit(0); }

    if (strstr(result,"United Kingdom")!=NULL) {
      printf(ENGLISH_LANG); exit(0); }

    if (strstr(result,"France")!=NULL) {
       printf(FRENCH_LANG); exit(0); }

    if (strstr(result,"Italy")!=NULL) {
      printf(ITAL_LANG); exit(0); }

    if (strstr(result,"Italy")!=NULL) {
      printf(ITAL_LANG); exit(0); }

    if (strstr(result,"Italy")!=NULL) {
      printf(SPANISH_LANG); exit(0); }

    printf(DEFAULT_LANG);
    return 0;
}

