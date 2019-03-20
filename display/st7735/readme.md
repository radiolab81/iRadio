# tft_st7735

## About
Library for the Raspberry Pi to display some stuff on a 1,8" TFT.
Released under the LGPL license.

This library provides basic functions to initialize the ST7735 Controller and draw text, lines and circles
and text fields on the display. Communication between the Raspberry Pi and the ST7735 is managed by the
wiringPi library (see http://projects.drogon.net/raspberry-pi/wiringpi/)

The source code in tft_st7735.cpp/h is based on several examples from the internet, mostly for the arduino.

## Required Hardware
* Raspberry Pi
* 1,8" TFT from Sainsmart

## Setup
* Install wiringPi. See http://projects.drogon.net/raspberry-pi/wiringpi/
* Checkout this project
* Build it with make [install]. 'Install' will install the library and includes to usr/local/lib and usr/local/include
* Additional make directives: clean install-static uninstall

## Usage
*  see examples

## Hints
* Only initR is tested, initG is for a different display
