#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

gcc -lm -lwiringPi -o rda5807 rda5807.c
cp ./rda5807 /usr/bin

cd st7735_demo_displayd
make
make install

cd gpiod
./build.sh

sudo killall gpiod
cp ./gpiod /usr/bin

cd ..
cd displayd
./build.sh

killall displayd
cp ./displayd /usr/bin

echo "Aenderungen sind nach dem Neustart aktiv!"
