#!/usr/bin/env bash

g++ displayd.c -o displayd -I/usr/local/include -lwiringPi -ltft_st7735 -lbcm2835

