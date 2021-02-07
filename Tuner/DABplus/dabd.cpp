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
#include <vector>
#include <algorithm>

#include <unistd.h>

using namespace std;

#define MUXFILE "/home/pi/iRadio/Tuner/DABplus/mux.list"
#define LABELFILE "/home/pi/iRadio/Tuner/DABplus/label.list"
#define URLFILE "/home/pi/iRadio/Tuner/DABplus/url.list"

vector<string> used_mux;
vector<string> used_label;
vector<string> used_url;

int iChannelIndex = 0;
bool bDABon = 0;

string cmd_opt = " --intf dummy --extraintf rc:http --rc-host 0.0.0.0:9294 --rc-fake-tty --http-host 0.0.0.0 --http-port 8080 --http-password raspberry --no-quiet --daemon ";

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}


void dbg_print_channel(int i) {
   cout << "****************************service-info****************************\n";
   cout << "Mux:" << used_mux[i] << "\n";
   cout << "Servicelabel:" << used_label[i] << "\n";
   cout << "connect to dabserver-url:" << used_url[i] << "\n";
   cout << "********************************************************************\n";
}
static void *socket_thread_tcp_ip_controll(void *arg) {
        int port = 9914;
        int r, n;
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        unsigned char buffer[512];
        struct sockaddr_in serv_addr, cli_addr;

        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sockfd < 0) {
                perror("ERROR opening socket");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *) &serv_addr,  sizeof(serv_addr)) < 0) {
                perror("ERROR on binding");
        }

        bzero(buffer,5);

        fprintf (stderr, "Main socket started on UDP/%d \n", port);

        while((n = read(sockfd,buffer,4)) != 0) {
                printf("%s \n",buffer);
                fflush(stdout);

		// DAB off
                if (strcmp((const char *)buffer,(const char *) "dab0") == 0 ) {
                        fprintf(stderr,"DABModus stby.\n");
                        system("killall welle-cli &");
		        system("killall vlc &");
			bDABon = 0;
                }

		// DAB on
                if (strcmp((const char *)buffer,(const char *) "dab1") == 0 ) {
                        fprintf (stderr,"DABModus aktiv.\n");
                        system("killall welle-cli &");
                        system("killall vlc &");
			string cmd = "welle-cli -c " + used_mux[iChannelIndex] + " -w 2345 &";
			cout << "Kommando: " << cmd << "\n";
			system(cmd.c_str());
			cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" + "DAB " + used_mux[iChannelIndex] + " - " + used_label[iChannelIndex] +"\" " + cmd_opt + " &";
			cout << "Kommando: " << cmd << "\n";
			usleep(6000000);
			system(cmd.c_str());

			dbg_print_channel(iChannelIndex);
			bDABon = 1;
                }

		// next prg
                if (strcmp((const char *)buffer,(const char *) "next") == 0 ) {
		   if (bDABon) {
			string old_mux = used_mux[iChannelIndex];

			if (iChannelIndex < used_url.size()-1)
			   iChannelIndex++;
			else
			   iChannelIndex=0;

		        if (old_mux.compare(used_mux[iChannelIndex])==0) {
			   // same mux
			   system("killall vlc");
			   //string cmd = "cvlc " + used_url[iChannelIndex] + " &";
			   //string cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" +used_label[iChannelIndex] +"\" " + cmd_opt + " &";
			   string cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" + "DAB " + used_mux[iChannelIndex] + " - " + used_label[iChannelIndex] +"\" " + cmd_opt + " &";

    			   system(cmd.c_str());
			}
			else // another mux
			{
                           system("killall welle-cli &");
                           system("killall vlc");
                           string cmd = "welle-cli -c " + used_mux[iChannelIndex] + " -w 2345 &";
                           system(cmd.c_str());
                           //cmd = "cvlc " + used_url[iChannelIndex] + " &";
                           cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" +used_label[iChannelIndex] +"\" " + cmd_opt + " &";
			   usleep(6000000);
                           system(cmd.c_str());
			}
			dbg_print_channel(iChannelIndex);
 		   }
                }

                // prev prg
                if (strcmp((const char *)buffer,(const char *) "prev") == 0 ) {
		   if (bDABon) {
                        string old_mux = used_mux[iChannelIndex];

                        if (iChannelIndex == 0)
                           iChannelIndex=used_url.size()-1;
                        else
                           iChannelIndex--;

                        if (old_mux.compare(used_mux[iChannelIndex])==0) {
                           // same mux
                           system("killall vlc");
                           //string cmd = "cvlc " + used_url[iChannelIndex] + " &";
			   //string cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" +used_label[iChannelIndex] +"\" " + cmd_opt + " &";
                           string cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" + "DAB " + used_mux[iChannelIndex] + " - " + used_label[iChannelIndex] +"\" " + cmd_opt + " &";

                           system(cmd.c_str());
                        }
                        else // another mux
                        {
                           system("killall welle-cli &");
                           system("killall vlc");
                           string cmd = "welle-cli -c " + used_mux[iChannelIndex] + " -w 2345 &";
                           system(cmd.c_str());
                           //cmd = "cvlc " + used_url[iChannelIndex] + " &";
		           //cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" +used_label[iChannelIndex] +"\" " + cmd_opt + " &";
 		           cmd = "cvlc " + used_url[iChannelIndex] + " --meta-title=\"" + "DAB " + used_mux[iChannelIndex] + " - " + used_label[iChannelIndex] +"\" " + cmd_opt + " &";
  			   usleep(6000000);
                           system(cmd.c_str());
                        }
                        dbg_print_channel(iChannelIndex);
		    }
                }


        }

        close(sockfd);
        return 0;
}



int main(int argc, char *argv[]) {

        char Buffer[255];

        ifstream in(MUXFILE);
	if(!in) {
 		cout << "Muxliste nicht gefunden/kann nicht geoeffnet werden.\n";
	    	return 1;
  	}

	while(in) {
	    	in.getline(Buffer, 255);  // delim defaults to '\n'
    		if(in) {
            		string str = Buffer;
      			used_mux.push_back(str);
     		}
  	}

	in.close();
        cout << "Muxliste enthaelt " << used_mux.size() << " Lines.\n";

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        ifstream in2(LABELFILE);
        if(!in2) {
                cout << "Labelliste nicht gefunden/kann nicht geoeffnet werden.\n";
                return 1;
        }

        while(in2) {
                in2.getline(Buffer, 255);  // delim defaults to '\n'
                if(in2) {
                        string str = Buffer;
                        used_label.push_back(str);
                }
        }

        in2.close();
        cout << "Labelliste enthaelt " << used_label.size() << " Lines.\n";

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        ifstream in3(URLFILE);
        if(!in3) {
                cout << "URLliste nicht gefunden/kann nicht geoeffnet werden.\n";
                return 1;
        }

        while(in3) {
                in3.getline(Buffer, 255);  // delim defaults to '\n'
                if(in3) {
                        string str = Buffer;
                        used_url.push_back(str);
                }
        }

        in3.close();
        cout << "URLliste enthaelt " << used_url.size() << " Lines.\n";


	// Interprozesskom herstellen
    	pthread_t process;
    	pthread_create(&process, NULL, socket_thread_tcp_ip_controll, NULL);
	pthread_detach(process);

	// Daemonbetrieb
        while(1) {


	usleep(1000000);
   	}
}

