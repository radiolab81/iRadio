#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wiringPi.h>

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unistd.h>

#include <ctime>

using namespace std;

#define  PowerPin  4  // GPIO-Pin 23, Pinnummer 16

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


int main(void) {

    if(wiringPiSetup() < 0){
	fprintf(stderr, "Unable to setup wiringPi:%s\n",strerror(errno));
	return 1;
     }

    // Schalter NF-Stufe
    pinMode(PowerPin, OUTPUT);

    while(1) {
    	// Weckzeit einlesen
    	read("/home/pi/iRadio/weckzeit.txt",wecker);

	// current date/time based on current system
   	time_t now = time(0);
	tm *ltm = localtime(&now);

	if (ltm->tm_hour == wecker.stunde)
	  if (ltm->tm_min == wecker.minute) {
		system("echo \"play\" | nc 127.0.0.1 9294 -N");
		system("echo \"netr\" | nc -u 127.0.0.1 6030 -w 0");
		digitalWrite(PowerPin,1);
	  }

	delay(40000); // alle 40 Sekunden
    }
}
