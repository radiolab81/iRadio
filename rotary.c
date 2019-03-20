#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#define  RoAPin    0  // GPIO-Pin 17, Pinnummer 11
#define  RoBPin    1  // GPIO-Pin 18, Pinnummer 12

unsigned char flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

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
			system("echo \"next\" | nc 127.0.0.1 9294 -N");
		}
		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			system("echo \"prev\" | nc 127.0.0.1 9294 -N");
		}

	}
}


int main(void)
{
	if(wiringPiSetup() < 0){
		fprintf(stderr, "Unable to setup wiringPi:%s\n",strerror(errno));
		return 1;
	}

	pinMode(RoAPin, INPUT);
	pinMode(RoBPin, INPUT);

	while(1){
		rotaryDeal();
        delay(10);
	}

	return 0;
}

