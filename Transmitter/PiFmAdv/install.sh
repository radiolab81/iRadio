#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

sudo apt-get update
sudo apt-get install libsndfile1-dev

sudo echo "gpu_freq=250"  >> /boot/config.txt
cd src
make clean 
make

cp ./pi_fm_adv /usr/bin/
cp ./vlcd /usr/bin/


