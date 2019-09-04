#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi


make -C /home/pi/iRadio/display/eink/wiringpi/
cd /home/pi/iRadio/display/eink/wiringpi

killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/eink/wiringpi/displayd /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

