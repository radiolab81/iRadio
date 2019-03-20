#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

using namespace std;

#define PLAYLIST "/home/pi/.config/vlc/playlist.m3u"
#define STATIONLABEL "/home/pi/.config/vlc/stations.txt"


void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

string get_Stationname() {
  char buf[64];
  FILE *text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r");
  fgets(buf, 64, text);
  rmSubstr(buf,"| title:");
  pclose(text);
  return buf;
}

int main(void) {

    char SenderURL[255];
    vector<string> Senderliste;
    vector<string> Namensliste; 
	
	pthread_t process1;

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

	// ermittle für alle URLs den Stationsnamen
	for (int i=0; i<Senderliste.size(); i++) {
	   string cmd = "/usr/bin/cvlc --extraintf rc:http --rc-host 0.0.0.0:9294 --rc-fake-tty ";
       cmd+= Senderliste.at(i);
       cmd+= " &";
	   system(cmd.c_str());	

	   sleep(5);
       Namensliste.push_back(get_Stationname());
	   cout << "Stationsname: " << Namensliste.at(i) << "\n";	

	   system("echo \"shutdown\" | nc 127.0.0.1 9294 -N ");
       sleep(1);
    }

    cout << "Anzahl Sender in Senderliste: " << Senderliste.size() << "\n";	
	cout << "Anzahl Stationsnamen in Namensliste: " << Namensliste.size() << "\n";	

	if (Senderliste.size() != Namensliste.size()) {
		cout << "Fehler bei der Namensaufloesung! \n";
	    return 1;
	}

	ofstream out(STATIONLABEL, ios::out | ios::trunc);
	if(!out) {
 		cout << "Namensliste konnte nicht angelegt werden.\n";
	    return 1;
  	}

    for (int i=0; i<Namensliste.size(); i++) {
	  if (Namensliste.at(i).empty())
		out << " noname " << i << "\n";
	  else 
		out << Namensliste.at(i);
	}

    out.close();

	cout << "Liste mit Stationsnamen wurde aktualisiert und syncronisiert.\n";
}
