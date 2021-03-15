#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <unistd.h>

using namespace std;

volatile bool bDevAvail = false;
volatile bool bMediaplayer_running = false;

static void *process_chkmount(void *arg) {
   char status[64];
   FILE *ret;

   while(1) {
     ret = popen("chkmount.sh", "r");
     fgets(status, 64, ret);
	
     if (strstr(status,"/dev")!=NULL) 
       //cout << "mountet\n";
       bDevAvail = true;
     else
       //cout << "nicht gemountet\n";
       bDevAvail = false;

     status[0]='\0';

     if (ret!=NULL) {
	pclose(ret);
	ret = NULL;
     }
    
     sleep(5);

   }
   return 0;
}


int main(int argc, char *argv[]) {

  // Interprozesskom herstellen
  pthread_t process;
  pthread_create(&process, NULL, process_chkmount, NULL);
  pthread_detach(process);

  // Daemonbetrieb
  while(1) { 
    // ext. USB-Laufwerk verfuegbar, Mediadateien listen und Playlist erzeugen, vlc mit dieser Playlist starten
    if (bDevAvail) {
       if (bMediaplayer_running==false) {
	 system("mkplaylist.sh");
	 system("killall vlc");
	 system("mpvlcd");
	 bMediaplayer_running=true;
       }
    } else {
       // zurueck zum vlc-Internetradiobetrieb 
       if (bMediaplayer_running) {	 
	 system("killall vlc");
         bMediaplayer_running=false;
	 system("vlcd");
       }
    }
    sleep(5);
  }
}

