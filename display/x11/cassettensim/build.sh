#!/usr/bin/env bash

g++ -D_REENTRANT sdlskale.cxx -o sdlskale -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread

gcc stby_gpiod.c -o stby_gpiod -lbcm2835
