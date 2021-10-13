#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

g++ /home/pi/iRadio/speakingd/displayd.cpp -o /home/pi/iRadio/speakingd/displayd

killall displayd
sleep 10

echo "Kopiere neuen Daemonen..."

cp /home/pi/iRadio/speakingd/displayd /usr/bin/

echo "Aenderungen sind nach dem Neustart aktiv!"

