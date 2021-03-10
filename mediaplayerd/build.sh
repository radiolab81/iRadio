#!/usr/bin/env bash
# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

g++ mediaplayerd.cpp -lpthread -o mediaplayerd

sudo killall mediaplayerd
cp ./mediaplayerd /usr/bin
cp ./chkmount.sh /usr/bin
cp ./mkplaylist.sh /usr/bin
cp ./mpvlcd /usr/bin

echo "ACHTUNG: mediaplayerd in Startdateien aktivieren!"
echo "Aenderungen werden nach Neustart aktiv!"
