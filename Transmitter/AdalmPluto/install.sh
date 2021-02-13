#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

apt install libxml2 libxml2-dev bison flex cmake git libaio-dev libboost-all-dev
apt install doxygen
apt install libusb-1.0-0-dev
apt install bison flex cmake git libgmp-dev

cd libiio
cmake .
make 
sudo make install
cd ..

cd libad9361-iio
cmake .
make 
sudo make install
cd ..

cd gr-iio
cmake .
make 
sudo make install
cd ..
sudo ldconfig
