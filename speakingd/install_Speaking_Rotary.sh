#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

gcc /home/pi/iRadio/speakingd/rotary.c -o /home/pi/iRadio/speakingd/gpiod -lwiringPi

killall gpiod
sleep 10

echo "Kopiere neuen Daemonen..."

cp /home/pi/iRadio/speakingd/gpiod /usr/bin/

echo "Aenderungen sind nach dem Neustart aktiv!"

