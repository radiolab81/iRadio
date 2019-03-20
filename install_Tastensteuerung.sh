#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

gcc /home/pi/iRadio/gpiod.c -o /home/pi/iRadio/gpiod -lbcm2835

killall gpiod
sleep 10

echo "Kopiere neuen Daemonen..."

cp /home/pi/iRadio/gpiod /usr/bin/

echo "Aenderungen sind nach dem Neustart aktiv!"
