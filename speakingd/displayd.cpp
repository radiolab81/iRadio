#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <iostream>


using namespace std;


void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

int main(int argc, char *argv[]) {
	string cmd;

	char title[64], title_old[64];


	FILE *text;
	// Daemonbetrieb
        while(1) {

       	   	text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r");

    		fgets(title, 64, text);

    		rmSubstr(title,"| title:");
    		rmSubstr(title,"\r");

    		title[strlen(title)-1]='\0';

		// wenn neuer Sendername ...
    		if ( (strcmp(title,title_old)!=0) ) {
		  
		    // de-DE , en-GB, en-US ... verschiedene Aussprachen
                    cmd = "pico2wave --lang=de-DE -w /tmp/pico.wav \"Sie hören ";
		    cmd += title;
		    cmd += "\" | aplay \0";
                    cout << cmd << "\n";
		    fflush(stdout);
		    system(cmd.c_str());

    		}

    		strcpy(title_old,title);

		if (text != NULL) {
    			pclose(text);
			text = NULL;
		}


    	 sleep(5);
   	}
}

