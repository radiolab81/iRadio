#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

apt-get update
apt-get install vlc-nox vlc-bin
apt-get install autoconf
apt-get install automake-1.15

if [ $? -ne 0 ] # apt-get return an error
then
   echo "Fehler: ($?): Fehler bei der Installation von vlc-nox"
   echo "Installationsscript wird abgebrochen, bitte prüfen Sie Ihre"
   echo "Internetverbindung und starten Sie das Installationscript neu."
   break
fi

mkdir /etc/vlcd/
cp default.m3u /etc/vlcd

cd bcm2835-1*
./configure
aclocal
make
make check
make install

gcc /home/pi/iRadio/gpiod.c -o /home/pi/iRadio/gpiod -lbcm2835

#cd /home/pi/iRadio/wiringPi
#./build
sudo dpkg -i /home/pi/iRadio/wiringpi-latest.deb

cd /home/pi/iRadio/autoStation
./build.sh

gcc /home/pi/iRadio/display/dummy.c -o /home/pi/iRadio/displayd -lbcm2835

sudo killall -9 vlcd
sudo killall -9 gpiod
sudo killall -9 displayd
sleep 5

cp /home/pi/iRadio/vlcd  /usr/bin/
cp /home/pi/iRadio/gpiod  /usr/bin/
cp /home/pi/iRadio/displayd /usr/bin/
cp /home/pi/iRadio/autoStation/autoStation /usr/bin/

if [ -f /etc/rc.local ]; then
   cp -f /etc/rc.local /etc/rc.local.bak
   cp /home/pi/iRadio/rc.local /etc/
fi

echo "Installerscript durchgelaufen, bitte Radio neu starten."

