#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unistd.h>

using namespace std;

#define SERVO_PIN 15 // GPIO15  - PWM-Anschluß zum Servo
#define TRIGGER_SERVO_PIN 24 // GPIO24 - Freigabe der Versorgungsspannung Servo

#define MIN_SERVO_PULSBREITE 1  // Zeigeranschlag links
#define MAX_SERVO_PULSBREITE 30  // Zeigeranschlag rechts

#define GESAMT_PULSBREITE 200 


#define PLAYLIST "/home/pi/.config/vlc/playlist.m3u"
//#define PLAYLIST "playlist.m3u"

#define ANZAHL_SENDER (Senderliste.size()-1)

#define SENDERABSTAND (MAX_SERVO_PULSBREITE-MIN_SERVO_PULSBREITE)/ANZAHL_SENDER

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

static void vlc_Callback()
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
}


int aktueller_Sender(vector<string> Liste, string URL) {
   //cout << "URL ist:" <<  URL << " size: "  << URL.size() << "\n";
   for (int i=0; i<Liste.size(); i++) {
		if (URL.rfind(Liste.at(i)) != -1)
	     	return i;
   }

  return -1;
}

int main () {
    char SenderURL[255];
    vector<string> Senderliste;
    int i_alt, i, skalenzeigerpos;
    
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


	if (wiringPiSetupGpio () == -1)
	{
	    fprintf (stdout, "Fehler Initialisierung WiringPi: %s\n", strerror (errno)) ;
	    return 1 ;
	}

	softPwmCreate(SERVO_PIN,0,GESAMT_PULSBREITE);
	softPwmWrite(SERVO_PIN,1);

	pinMode(TRIGGER_SERVO_PIN, OUTPUT);
    digitalWrite (TRIGGER_SERVO_PIN, LOW);

	skalenzeigerpos = 0;
	// Daemonbetrieb
	while(1) {
        vlc_Callback();
  	    // welcher Sender wird von vlc gerade wiedergeben ?
    	string str = streamURL;

		i_alt = i;
    	i = aktueller_Sender(Senderliste,str);

		// URL wegen URL Redirection nicht in Senderliste? Dann belasse Zeiger an Stelle.
		if (i == -1)
			i = i_alt;

    	// Verschiebe Skalenzeigerposition zum eingestellten Sender
    	if (skalenzeigerpos != (SENDERABSTAND*i)) {
        	skalenzeigerpos = SENDERABSTAND*i;
	  	}

		// Ansteuerung Servo
        if (i != i_alt) {
			digitalWrite (TRIGGER_SERVO_PIN, HIGH);
     		softPwmWrite(SERVO_PIN,skalenzeigerpos+1);
		}
		
		sleep(2);	
 		digitalWrite (TRIGGER_SERVO_PIN, LOW);
	}	
 
}
