#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

sudo apt-get install libpopt-dev
sudo dpkg -i pico2wave.deb
ln -s /dev/stdout /tmp/pico.wav
pico2wave --lang=de-DE -w /tmp/pico.wav "Hallo die Installation der Sprachausgabe war erfolgreich." | aplay

cp /home/pi/iRadio/speakingd/speakingd.sh /usr/bin/
