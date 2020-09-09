#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Dial.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Tabs.H>


static Fl_Group* stc_iRadio = NULL;
static Fl_Group* stc_Tuner = NULL;

static bool bInternetradioOn = true;

struct SDR_Einstellung {
    long frequenz; // Frequenz in Hz
    int gain;  // MMIC-Verstaerung xx.xx dB
    short modulation; // 0 - FM , 1 - AM, 2 - USB, 3 - LSB
    int squelch; // 0 - aus .... 255 voll
    bool agc; // 0 - aus , 1 - an
};

struct SDR_Radio {
    Fl_Box *box;
    Fl_Dial *freq;
    Fl_Dial *gain;
    Fl_Dial *squelch;
    SDR_Einstellung sdr;
};

static SDR_Radio* sdr = new  SDR_Radio;
static double dOldDialValue;


// GUI-Handler fuer Tunertab
void hndl_freqaenderung(Fl_Widget *, void* data) {
   char freq[64];
   long lFreqstep;

   SDR_Radio *hndl = (SDR_Radio*) data;
   Fl_Dial *dial = hndl->freq;

   if (dial->value() > dOldDialValue)
        if (hndl->sdr.frequenz > 108000000)
           lFreqstep = 8300;  // 8.33 kHz Kanalraster
        else
           lFreqstep = 100000; // 100 kHz Kanalraster
   else
        if (hndl->sdr.frequenz > 108000000)
           lFreqstep = -8300;
        else
           lFreqstep = -100000;

   hndl->sdr.frequenz = hndl->sdr.frequenz + lFreqstep;
   dOldDialValue =  dial->value();
   // Baue Kommando zur Frequenzaenderung ./udpclient freq xxxxxxxxx [Hz]
   sprintf(freq, "%ld", hndl->sdr.frequenz);
   char cmd[32] = "./udpclient freq ";
   strcat(cmd,freq);
   system(cmd);

   // Automatischer Demodulatorwechsel bei 108 MHz
   if (hndl->sdr.frequenz > 108000000) {
       if (hndl->sdr.modulation == 0) {
         hndl->sdr.modulation=1;
         system("./udpclient mode 1");
       }
   } else {
       if (hndl->sdr.modulation == 1) {
         hndl->sdr.modulation=0;
         system("./udpclient mode 0");
       }
   }
}


void hndl_gain(Fl_Widget *, void* data) {
   char gain[8];

   SDR_Radio *hndl = (SDR_Radio*) data;
   Fl_Dial *dial = hndl->gain;
   hndl->sdr.gain = round(dial->value()*500);

   //  ./updclient gain xx.xx dB
   sprintf(gain, "%i", hndl->sdr.gain);
   char cmd[32] = "./udpclient gain ";
   strcat(cmd,gain);
   system(cmd);

}

void hndl_squelch(Fl_Widget *, void* data) {
   char squelch[8];

   SDR_Radio *hndl = (SDR_Radio*) data;
   Fl_Dial *dial = hndl->squelch;
   hndl->sdr.squelch = round(dial->value()*255);


   sprintf(squelch, "%i", hndl->sdr.squelch);
   char cmd[32] = "./udpclient squelch ";
   strcat(cmd,squelch);
   system(cmd);

}

// Displayaktualisierung Tuner alle 0.1 Sekunde
static void handler_timer_channel(void *data) {              // timer callback
if (bInternetradioOn==false) {
    char freq[16];

    SDR_Radio *hndl = (SDR_Radio*) data;
    Fl_Box *box = hndl->box;
    Fl_Dial *dial = hndl->freq;

    box->labelsize(24);

    setlocale(LC_ALL,"");
    sprintf(freq, "%'.2f", (double) hndl->sdr.frequenz/1000000);

    strcat(freq," MHz\n");


    switch(hndl->sdr.modulation) {
        case 0: strcat(freq, "FM"); break;
        case 1: strcat(freq, "AM"); break;
        case 2: strcat(freq, "USB"); break;
        case 3: strcat(freq, "LSB"); break;
    }

    box->copy_label(freq);
}
    Fl::repeat_timeout(0.1, handler_timer_channel, data);

}

// Funktion zum Umschalten zwischen iRadio und Tuner
void startiRadio() {
   bInternetradioOn = true;
   system("echo \"play\" | nc 127.0.0.1 9294 -N");
}

void startTuner() {
   bInternetradioOn = false;
   char freq[64];
   char cmd[32] = "./rtl_udpd.sh ";
   sprintf(freq, "%ld", sdr->sdr.frequenz);
   strcat(cmd,freq);
   strcat(cmd," &");
   system(cmd);
}
void stopAllAudio() {
   // stop iRadio
   system("echo \"stop\" | nc 127.0.0.1 9294 -N");
   // kill tunerprocess
   system("killall rtl_fm &");
}

// Gui-Handler fuer Tab-Wechsel - iRadio/Tuner
void MyTabCallback(Fl_Widget *w, void*) {
  Fl_Tabs *tabs = (Fl_Tabs*)w;
   printf("Umschaltung Empfangsmodus\n");
   if (tabs->value() == stc_iRadio) {
     printf("nach iRadio \n");
     stopAllAudio();
     startiRadio();
   } else {
     printf("nach Tuner \n");
     stopAllAudio();
     startTuner();
   }
}

// GUI-Handler fuer iRadio-Knoepfe
void hndl_btnplay(Fl_Widget *, void *) {
  system("echo \"play\" | nc 127.0.0.1 9294 -N");
}

void hndl_btnstop(Fl_Widget *, void *) {
  system("echo \"stop\" | nc 127.0.0.1 9294 -N");
}

void hndl_btnprevstation(Fl_Widget *, void *) {
  system("echo \"prev\" | nc 127.0.0.1 9294 -N");
}

void hndl_btnnextstation(Fl_Widget *, void *) {
  system("echo \"next\" | nc 127.0.0.1 9294 -N");
}

void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}

// Displayaktualisierung iRadio jede Sekunde
static void handler_timer_channel_iRadio(void *data) {              // timer callback
if (bInternetradioOn == true) {
    char buf[512];
    char now[64];
    char genre[64];
    char bitrate[64];

    //FILE *text =  popen("ls *", "r");
    FILE *text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r");
    FILE *text_now_playing = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep now_playing:", "r");
    FILE *text_genre = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep genre: ", "r");
    FILE *text_bitrate = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep Bitrate:", "r");

    Fl_Box *b = (Fl_Box*) data;

    b->labelsize(10);

    fgets(buf, 64, text);
    fgets(now, 64, text_now_playing);
    fgets(genre, 64, text_genre);
    fgets(bitrate, 64, text_bitrate);

    rmSubstr(buf,"| title:");
    rmSubstr(buf,"\r");

    rmSubstr(now,"| now_playing:");
    rmSubstr(now,"\r");

    rmSubstr(genre,"| genre:");
    rmSubstr(genre,"\r");

    rmSubstr(bitrate,"| Bitrate:");
    rmSubstr(bitrate,"\r");

    strcat(buf,now);
    strcat(buf,genre);
    strcat(buf,bitrate);
    b->copy_label(buf);
    //printf("%s",(char*)buf);

    pclose(text);
    pclose(text_now_playing);
    pclose(text_genre);
    pclose(text_bitrate);

}
    Fl::repeat_timeout(1, handler_timer_channel_iRadio, data);
}

int main(int argc, char **argv) {
  sdr->sdr.frequenz = 88000000;
  sdr->sdr.gain = 500;
  sdr->sdr.modulation = 0;
  sdr->sdr.squelch = 0;

  static unsigned int cur_volume = 0;

Fl_Window *window = new Fl_Window(480,290,"WorldRadio");

Fl_Tabs *tabs = new Fl_Tabs(10,100,460,270);
{
    tabs->callback(MyTabCallback);

    Fl_Group *iRadioGrp = new Fl_Group(20,120,460,240,"iRadio");
    {
      stc_iRadio = iRadioGrp;

      Fl_Button *btnplay = new Fl_Button(20, 140, 70 , 65, "@>");
      Fl_Button *btnstop = new Fl_Button(96, 140, 70, 65, "@||");

      Fl_Button *btnprevstation = new Fl_Button(20, 215, 70 , 65, "@<<");
      Fl_Button *btnnextstation = new Fl_Button(96, 215, 70, 65, "@>> ");

      btnplay->callback(hndl_btnplay,0);
      btnstop->callback(hndl_btnstop,0);

      btnprevstation->callback(hndl_btnprevstation,0);
      btnnextstation->callback(hndl_btnnextstation,0);

    }
    iRadioGrp->end();


    Fl_Group *tunerGrp = new Fl_Group(20,120,460,240,"Tuner");
    {
      stc_Tuner = tunerGrp;

	  Fl_Dial *dial_freq = new Fl_Dial(60,140,110,110,"Abstimmung");
          dial_freq->angles(0,30000);
          dial_freq->callback(hndl_freqaenderung,sdr);

	  Fl_Dial *dial_gain = new Fl_Dial(300,140,60,60,"Gain");
          dial_gain->angles(20,340);
          dial_gain->value(1);
          dial_gain->callback(hndl_gain,sdr);

	  Fl_Dial *dial_squelch = new Fl_Dial(380,140,60,60,"Squelch");
          dial_squelch->angles(0,359);
          dial_squelch->callback(hndl_squelch,sdr);


          sdr->freq = dial_freq;
          sdr->gain = dial_gain;
          sdr->squelch = dial_squelch;

    }
    tunerGrp->end();

}
tabs->end();

  Fl_Box *box_channel = new Fl_Box(10,10,460,80);
  box_channel->box(_FL_ROUND_DOWN_BOX );
  box_channel->labelfont(FL_BOLD+FL_ITALIC);
  box_channel->labelsize(18);
  box_channel->labeltype(FL_SHADOW_LABEL);
  box_channel->labelcolor(FL_GREEN);

  sdr->box = box_channel;
  Fl::add_timeout(0.1, handler_timer_channel, sdr);

  Fl::add_timeout(1, handler_timer_channel_iRadio, box_channel);
  //window->fullscreen();
  window->end();
  window->show(argc, argv);
  return Fl::run();
}

