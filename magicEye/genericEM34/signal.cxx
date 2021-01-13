//libraries necessary for wifi ioctl communication
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>

//struct to hold collected information
typedef struct  {
    char mac[18];
    char ssid[33];
    int bitrate;
    int level;
} signalInfo;

int getSignalInfo(signalInfo *sigInfo, char *iwname){
    struct iwreq req;
    strcpy(req.ifr_name, iwname);

    //struct iw_statistics *stats;

    //have to use a socket for ioctl
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //make room for the iw_statistics object
    req.u.data.pointer = (struct iw_statistics *)malloc(sizeof(struct iw_statistics));
    req.u.data.length = sizeof(struct iw_statistics);

    //this will gather the signal strength
    if(ioctl(sockfd, SIOCGIWSTATS, &req) == -1){
        //die with error, invalid interface
        fprintf(stderr, "Invalid interface.\n");
        return(-1);
    }
    else if(((struct iw_statistics *)req.u.data.pointer)->qual.updated & IW_QUAL_DBM){
        //signal is measured in dBm and is valid for us to use
        sigInfo->level=((struct iw_statistics *)req.u.data.pointer)->qual.level - 256;
    }


    //SIOCGIWESSID for ssid
    char buffer[32];
    memset(buffer, 0, 32);
    req.u.essid.pointer = buffer;
    req.u.essid.length = 32;
    //this will gather the SSID of the connected network
    if(ioctl(sockfd, SIOCGIWESSID, &req) == -1){
        //die with error, invalid interface
        return(-1);
    }
    else{
        memcpy(&sigInfo->ssid, req.u.essid.pointer, req.u.essid.length);
        memset(&sigInfo->ssid[req.u.essid.length],0,1);
    }

    //SIOCGIWRATE for bits/sec (convert to mbit)
    int bitrate=-1;
    //this will get the bitrate of the link
    if(ioctl(sockfd, SIOCGIWRATE, &req) == -1){
        fprintf(stderr, "bitratefail");
        return(-1);
    }else{
        memcpy(&bitrate, &req.u.bitrate, sizeof(int));
        sigInfo->bitrate=bitrate/1000000;
    }


    //SIOCGIFHWADDR for mac addr
    struct ifreq req2;
    strcpy(req2.ifr_name, iwname);
    //this will get the mac address of the interface
    if(ioctl(sockfd, SIOCGIFHWADDR, &req2) == -1){
        fprintf(stderr, "mac error");
        return(-1);
    }
    else{
        sprintf(sigInfo->mac, "%.2X", (unsigned char)req2.ifr_hwaddr.sa_data[0]);
        for(int s=1; s<6; s++){
            sprintf(sigInfo->mac+strlen(sigInfo->mac), ":%.2X", (unsigned char)req2.ifr_hwaddr.sa_data[s]);
        }
    }
    close(sockfd);
  return 0;
}

int main()
{
  signalInfo sigInfo;
  getSignalInfo(&sigInfo, "wlan0");
  int sig_qual = 0;
  if (sigInfo.level < -110)
			sig_qual = 0;
		else if (sigInfo.level > -40)
			sig_qual = 70;
		else
			sig_qual = sigInfo.level + 110;

/*  printf("mac address: %s\nSSID: %s\nbitrate: %d\nLevel: %d\nQuality: %d/70 %0.f%%\n",
    sigInfo.mac,
    sigInfo.ssid,
    sigInfo.bitrate,
    sigInfo.level,
    sig_qual,
    (1e2 * sig_qual) / 70
  );
*/

printf("%0.f\n",(1e2 * sig_qual) / 70);



return 0;
}
