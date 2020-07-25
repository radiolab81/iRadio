#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#include <pthread.h>

#include <iostream>
#include <fstream>


using namespace std;

#define  RoAPin    0  // GPIO-Pin 17, Pinnummer 11
#define  RoBPin    1  // GPIO-Pin 18, Pinnummer 12
#define  RoSWPin   15 // Pinnummer 8

#define  PowerPin  4  // GPIO-Pin 23, Pinnummer 16

unsigned char flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

static int bRadioAn = 1;   // 0 - Radio aus ; 1 - Radio an

struct Weckzeit{ // The object to be serialized / deserialized
public:
  	int stunde;
  	int minute;
};

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


Weckzeit wecker;


void rotaryDeal(void)
{
	Last_RoB_Status = digitalRead(RoBPin);

	while(!digitalRead(RoAPin)){
		Current_RoB_Status = digitalRead(RoBPin);
		flag = 1;
	}

	if(flag == 1){
		flag = 0;
		if((Last_RoB_Status == 0)&&(Current_RoB_Status == 1)){
			if (bRadioAn)
				system("echo \"next\" | nc 127.0.0.1 9294 -w 0");
			else {
			    //wecker.minute++;
			    wecker.minute = wecker.minute + 5;
			    if (wecker.minute == 60) {
				wecker.minute = 0;
			        wecker.stunde++;
			        if (wecker.stunde == 24)
				   wecker.stunde = 0;
			    }
			    write("/home/pi/iRadio/weckzeit.txt",wecker);
			    system("echo \"weck\" | nc -u 127.0.0.1 6030 -w 0");

			}
		}

		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			if (bRadioAn)
				system("echo \"prev\" | nc 127.0.0.1 9294 -w 0");
			else {
			   wecker.minute = wecker.minute - 5;
			   if (wecker.minute == -5) {
			      wecker.minute = 55;
  			      wecker.stunde--;
			      if (wecker.stunde == -1)
			         wecker.stunde = 23;
			   }
                            write("/home/pi/iRadio/weckzeit.txt",wecker);
                            system("echo \"weck\" | nc -u 127.0.0.1 6030 -w 0");
			}
		}

	}
}

static void *thread_modiswitch(void *arg) {
	int time_ms_by100 = 0;

	while(1) {

		while(!digitalRead(RoSWPin)){
			time_ms_by100++;
			delay(100);
		}

		// zwischen 500 und 1200 ms, an/aus
		if ((time_ms_by100 > 5) && (time_ms_by100 < 12)) {
			if (bRadioAn) {
	    	    system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
				system("echo \"stby\" | nc -u 127.0.0.1 6030 -w 0");
				digitalWrite(PowerPin,0);
			}
			else {
    		    system("echo \"play\" | nc 127.0.0.1 9294 -N");
				system("echo \"netr\" | nc -u 127.0.0.1 6030 -w 0");
				digitalWrite(PowerPin,1);
			}
		}

		// laenger als 2500 ms, Wecker an/aus
		if (time_ms_by100 > 25) {
			// ist Wecker an? alarman.status existiert
			ifstream in("/home/pi/.config/vlc/alarman.status");
			if(in) {
				in.close();
				// Wecker aus.
				system("rm /home/pi/.config/vlc/alarman.status");
			}
			else {
				// Wecker an.
				system("touch /home/pi/.config/vlc/alarman.status");
			}
		}

		time_ms_by100 = 0;

		delay(10);
	}

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

static void *thread_isVLCPlaying(void *arg) {
	char statusPlaying[65];

	while(1) {
		FILE *text_statusPlaying = popen("echo \"status\" | nc 127.0.0.1 9294 -N | grep state", "r");
		fgets(statusPlaying,64, text_statusPlaying);
	    	rmSubstr(statusPlaying,"( state ");
    		rmSubstr(statusPlaying," )");
		rmSubstr(statusPlaying," ");
		pclose(text_statusPlaying);

 		if (strstr(statusPlaying,"playing") != NULL)
			bRadioAn = 1;

  		else {
			bRadioAn = 0;
		}

		delay(100);
	}

	return 0;
}


int main(void)
{
	if(wiringPiSetup() < 0){
		fprintf(stderr, "Unable to setup wiringPi:%s\n",strerror(errno));
		return 1;
	}

	// Weckzeit einlesen
	read("/home/pi/iRadio/weckzeit.txt",wecker);

	pinMode(RoAPin, INPUT);
	pinMode(RoBPin, INPUT);
	pinMode(RoSWPin, INPUT);

	// Schalter NF-Stufe
	pinMode(PowerPin, OUTPUT);
	digitalWrite(PowerPin,1);


    	pthread_t process_rotary_switch, process_isVLCPlaying;
    	pthread_create(&process_rotary_switch, NULL, thread_modiswitch, NULL);
	pthread_detach(process_rotary_switch);

	pthread_create(&process_isVLCPlaying, NULL, thread_isVLCPlaying, NULL);
	pthread_detach(process_isVLCPlaying);

	while(1){
		rotaryDeal();
        delay(10);
	}

	return 0;
}

