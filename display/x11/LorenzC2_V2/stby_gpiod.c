// gpiod.c
//
// Beispielprogramm fuer bcm2835 lib
// Schaltet per GPIO-Taster nach Masse einen laufenden VLC-Prozess (next, prev, ...)
//
// After installing bcm2835, you can build this 
// with something like:
// gcc  gpiod.c -o gpiod -lbcm2835
// sudo ./gpiod
//
// Author: Bernhard45 (mbernhard1945@gmail.com)

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//#define DEBUG

// Tasteranschluesse
#define PIN_PRG_HOCH RPI_GPIO_P1_11
#define PIN_PRG_RUNTER RPI_GPIO_P1_12
#define PIN_PRG_AUS RPI_GPIO_P1_08
#define PIN_PRG_AN RPI_GPIO_P1_10

int main(int argc, char **argv)
{
	int bRadioAn = 1;   // 0 - Radio aus ; 1 - Radio an

    if (!bcm2835_init())
	return 1;

    // Pin als Eingang setzen , mit Pull-Up
    bcm2835_gpio_fsel(PIN_PRG_HOCH, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN_PRG_HOCH,BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(PIN_PRG_RUNTER, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN_PRG_RUNTER,BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(PIN_PRG_AUS, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN_PRG_AUS,BCM2835_GPIO_PUD_UP);

    bcm2835_gpio_fsel(PIN_PRG_AN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(PIN_PRG_AN,BCM2835_GPIO_PUD_UP);

    // Endlosschleife, da Daemonbetrieb
    while (1)
    {
	// Zustand am Pin einlesen
	uint8_t val_prg_hoch   = bcm2835_gpio_lev(PIN_PRG_HOCH);
	uint8_t val_prg_runter = bcm2835_gpio_lev(PIN_PRG_RUNTER);
    uint8_t val_prg_aus    = bcm2835_gpio_lev(PIN_PRG_AUS);
    uint8_t val_prg_an	   = bcm2835_gpio_lev(PIN_PRG_AN);

	#ifdef DEBUG
		printf("Tastenwert PIN_HOCH: %d\n", val_prg_hoch);
		printf("Tastenwert PIN_RUNTER: %d\n", val_prg_runter);
		printf("Tastenwert PIN_AUS: %d\n", val_prg_aus);
		printf("Tastenwert PIN_AN: %d\n", val_prg_an);
	#endif

	if (val_prg_hoch == 0) {
        if (bRadioAn)
	  	  	system("echo \"next\" | nc 127.0.0.1 9294 -w 0");
	}

    if (val_prg_runter == 0) {	
		if (bRadioAn)
        	system("echo \"prev\" | nc 127.0.0.1 9294 -w 0");
	}

    if (val_prg_aus == 0) {
		bRadioAn = 0;
        system("echo \"stop\" | nc 127.0.0.1 9294 -w 0");
		system("echo \"stby\" | nc -u 127.0.0.1 6030 -w 0");
	}

    if (val_prg_an == 0) {
		bRadioAn = 1;
        system("echo \"play\" | nc 127.0.0.1 9294 -N");
		system("echo \"netr\" | nc -u 127.0.0.1 6030 -w 0");
	}

    // for Bluetoothmode send system("echo \"blth\" | nc -u 127.0.0.1 6030 -w 0");

	delay(500);
    }

    return 0;
}
