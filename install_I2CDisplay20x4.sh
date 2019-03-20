#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

/home/pi/iRadio/setup_i2c.sh

make -C /home/pi/iRadio/display/lcd/

cp /home/pi/iRadio/display/lcd/lcdpcf8574.h /usr/local/include/lcdpcf8574.h
cp /home/pi/iRadio/display/lcd/liblcdpcf8574.so.1.0 /usr/local/lib/
ln -sf /usr/local/lib/liblcdpcf8574.so.1.0 /usr/local/lib/liblcdpcf8574.so
ln -sf /usr/local/lib/liblcdpcf8574.so.1.0 /usr/local/lib/liblcdpcf8574.so.1
ldconfig


killall -9 displayd
sleep 5

cp /home/pi/iRadio/display/lcd/displayd /usr/bin

echo "Aenderungen werden nach Neustart aktiv!"

