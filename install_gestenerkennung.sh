#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

/home/pi/iRadio/setup_i2c.sh

cd /home/pi/iRadio/Gesture/PAJ7620U2
make

killall gestured
cp /home/pi/iRadio/Gesture/PAJ7620U2/gestured  /usr/bin/

echo "Aktiviere bitte in /etc/rc.local den Eintrag /usr/bin/gestured & um die Gestenerkennung beim nächsten Systemstart einzuschalten!"




