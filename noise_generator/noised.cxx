#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

#include <iostream>

#include <thread>
#include <chrono>


using namespace std;

static int bRadioAn = 1;   // 0 - Radio aus ; 1 - Radio an
static int aplay_started = 0;

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

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	return 0;
}


int main(void)
{
	pthread_t process_isVLCPlaying;   
	pthread_create(&process_isVLCPlaying, NULL, thread_isVLCPlaying, NULL);
	pthread_detach(process_isVLCPlaying);

	while(1){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (!bRadioAn) {
			if (aplay_started == 0) {
			   system("aplay --channels=2 --rate=44100 /home/pi/iRadio/noise_generator/noise.mp3 &");
			   aplay_started = 1;
  			 }
		} 
        else {
            if (aplay_started == 1) {
			   system("sudo killall aplay");
               aplay_started = 0;
			}
		}

	}

	return 0;
}

