#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi


make -C /home/pi/iRadio/display/st7735/
cd /home/pi/iRadio/display/st7735/
make install

cd /home/pi/iRadio/display/st7735/displayd/
./build.sh

killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/st7735/displayd/displayd /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

