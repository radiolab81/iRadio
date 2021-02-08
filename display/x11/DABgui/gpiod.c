#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#define  RoAPin    0  // GPIO-Pin 17, Pinnummer 11
#define  RoBPin    1  // GPIO-Pin 18, Pinnummer 12
#define  RoSWPin   2
#define  SwDABscanPin 3

unsigned char flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

int time_ms_by10 = 0;
int time_ms_by10_2 = 0;

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
			system("echo \"next\" | nc -u 127.0.0.1 6030 -w 0");
		}
		if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
			system("echo \"prev\" | nc -u 127.0.0.1 6030 -w 0");
		}

	}
}

void rotarySW(void) {
   if (!digitalRead(RoSWPin)) {
	time_ms_by10++;
   } else {

       // zwischen 10ms und 1000ms
       if ((time_ms_by10>1) && (time_ms_by10<100)) {
         system("echo \"entr\" | nc -u 127.0.0.1 6030 -w 0");
       } 
   
      // zwischen 2000ms und 5000ms
      if ((time_ms_by10>200) && (time_ms_by10<500)) {
        system("echo \"menu\" | nc -u 127.0.0.1 6030 -w 0");
      } 
   
      // länger als 6000ms
      if (time_ms_by10>600) {
        system("echo \"menu\" | nc -u 127.0.0.1 6030 -w 0"); 
        system("echo \"stby\" | nc -u 127.0.0.1 6030 -w 0");
      } 

      time_ms_by10=0;
   }
}

void switchDABscan(void) {
   if (!digitalRead(SwDABscanPin)) {
     time_ms_by10_2++;
   } else {

      // ab 2000ms
      if (time_ms_by10_2>200) {
        system("echo \"dabs\" | nc -u 127.0.0.1 6030 -w 0");
      } 

      time_ms_by10_2=0;
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
		rotarySW();
		switchDABscan();
        delay(10);
	}

	return 0;
}

