#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Widget.H>


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

void hndl_btnvolup(Fl_Widget *, void *) {
  system("echo \"volup 2\" | nc 127.0.0.1 9294 -N");
}

void hndl_btnvoldown(Fl_Widget *, void *) {
  system("echo \"voldown 2\" | nc 127.0.0.1 9294 -N");
}

void hndl_btnmute(Fl_Widget *, void * cur_vol) {
  unsigned int *vol = (unsigned int*)cur_vol;

  if (*vol==0) {
    system("echo \"volume 250\" | nc 127.0.0.1 9294 -N");
    *vol = 250;
  }
  else {
    system("echo \"volume 0\" | nc 127.0.0.1 9294 -N");
    *vol = 0;
  }
}


void rmSubstr(char *str, const char *toRemove)
{
    size_t length = strlen(toRemove);
    while((str = strstr(str, toRemove)))
    {
        memmove(str, str + length, 1 + strlen(str + length));
    }
}


static void handler_timer_channel(void *data) {              // timer callback
    char buf[512];
    char now[64];
    char genre[64];
    char bitrate[64];

    //FILE *text =  popen("ls *", "r");
    FILE *text = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep title:", "r"
);
    FILE *text_now_playing = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep 
now_playing:", "r");
    FILE *text_genre = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep genre:
", "r");
    FILE *text_bitrate = popen("echo \"info\" | nc 127.0.0.1 9294 -N | grep Bitr
ate:", "r");

    Fl_Box *b = (Fl_Box*) data;

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

    Fl::repeat_timeout(1, handler_timer_channel, data);
}




int main(int argc, char **argv) {
  static unsigned int cur_volume = 0;

  Fl_Window *window = new Fl_Window(480,290,"iRadio");
  Fl_Button *btnplay = new Fl_Button(10, 140, 70 , 65, "@>");
  Fl_Button *btnstop = new Fl_Button(86, 140, 70, 65, "@||");
  Fl_Button *btnvolup = new Fl_Button(162, 140, 70, 65, "@+");
  Fl_Button *btnmute = new Fl_Button(240, 140, 70, 65, "Mute");

  Fl_Button *btnprevstation = new Fl_Button(10, 215, 70 , 65, "@<<");
  Fl_Button *btnnextstation = new Fl_Button(86, 215, 70, 65, "@>> ");
  Fl_Button *btnvoldown = new Fl_Button(162, 215, 70, 65, "@line");
  Fl_Button *btnexit = new Fl_Button(240, 215, 70, 65, "@returnarrow");

  btnplay->callback(hndl_btnplay,0);
  btnstop->callback(hndl_btnstop,0);

  btnprevstation->callback(hndl_btnprevstation,0);
  btnnextstation->callback(hndl_btnnextstation,0);

  btnvolup->callback(hndl_btnvolup,0);
  btnvoldown->callback(hndl_btnvoldown,0);

  btnmute->callback(hndl_btnmute,&cur_volume);

  Fl_Box *box_channel = new Fl_Box(10,10,460,100);
  box_channel->box(_FL_ROUND_DOWN_BOX );
  box_channel->labelfont(FL_BOLD+FL_ITALIC);
  box_channel->labelsize(24);
  box_channel->labeltype(FL_SHADOW_LABEL);
  box_channel->labelcolor(FL_GREEN);


  Fl::add_timeout(1, handler_timer_channel, box_channel);

  //window->fullscreen();
  window->end();
  window->show(argc, argv);
  return Fl::run();
}

