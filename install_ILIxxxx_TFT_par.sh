#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi


cd /home/pi/iRadio/display/ILIxxxx_parallel/
./build.sh

killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/ILIxxxx_parallel/pin.conf /usr/bin
cp /home/pi/iRadio/display/ILIxxxx_parallel/displayd /usr/bin
cp -R /home/pi/iRadio/display/ILIxxxx_parallel/fontx /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

