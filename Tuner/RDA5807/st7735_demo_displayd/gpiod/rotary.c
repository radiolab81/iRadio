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

static int modus = 0;   // 0 - Internetradio ; 1 - UKW Radio; 2 = Standby

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
			if (modus == 0)
			    system("echo \"next\" | nc 127.0.0.1 9294 -w 0");
			else {
			    system("rda5807 up");
			}
		}

		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			if (modus == 0)
			    system("echo \"prev\" | nc 127.0.0.1 9294 -w 0");
			else {
                            system("rda5807 down");
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

		// zwischen 500 und 1200 ms, Umschaltung Internetradio/UKW-Radio
		if ((time_ms_by100 > 5) && (time_ms_by100 < 12)) {
			if (modus == 0) {
	    	                //system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
				system("echo \"radi\" | nc -u 127.0.0.1 6030 -w 0");
				modus = 1;
				digitalWrite(PowerPin,1);
			}
			else {
    		    		//system("echo \"play\" | nc 127.0.0.1 9294 -N");
				system("echo \"netr\" | nc -u 127.0.0.1 6030 -w 0");
				modus = 0;
				digitalWrite(PowerPin,1);
			}
		}

		// laenger als 2500 ms, gehe in Stanby
		if (time_ms_by100 > 25) {
		  if (modus < 2) {
		     system("echo \"stby\" | nc -u 127.0.0.1 6030 -w 0");
		     modus = 2;
	             digitalWrite(PowerPin,0);
		  } else {
		     system("echo \"netr\" | nc -u 127.0.0.1 6030 -w 0");
                     modus = 0;
		     digitalWrite(PowerPin,1);
		  }
		}

		time_ms_by100 = 0;

		delay(10);
	}

	return 0;
}


int main(void)
{
	if(wiringPiSetup() < 0){
		fprintf(stderr, "Unable to setup wiringPi:%s\n",strerror(errno));
		return 1;
	}

	pinMode(RoAPin, INPUT);
	pinMode(RoBPin, INPUT);
	pinMode(RoSWPin, INPUT);

	// Schalter NF-Stufe
	pinMode(PowerPin, OUTPUT);
	digitalWrite(PowerPin,1);


    pthread_t process_rotary_switch, process_isVLCPlaying;
    pthread_create(&process_rotary_switch, NULL, thread_modiswitch, NULL);
	pthread_detach(process_rotary_switch);


	while(1){
      	   rotaryDeal();
           delay(10);
	}

	return 0;
}

