#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

/home/pi/iRadio/setup_i2c.sh

make -C /home/pi/iRadio/display/oled/
cd /home/pi/iRadio/display/oled/
make -f make_displayd

killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/oled/displayd /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

