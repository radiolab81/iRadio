#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi


cd /home/pi/iRadio/display/servo/
./build.sh

killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/servo/displayd /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

