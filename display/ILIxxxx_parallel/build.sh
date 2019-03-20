#!/usr/bin/env bash

cc -o displayd displayd.c fontx.c ili93xx.c -lwiringPi -lm -lpthread -DILI9481

