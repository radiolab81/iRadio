#!/usr/bin/env bash

# ueberpruefe auf sudo-Rechte
if [ "$(id -u)" != 0 ]; then
  echo "Zur Ausfuehrung dieses Scripts sind Administratorrechte noetig"
  exit 1
fi

sudo apt-get install libfaad-dev libmpg123-dev libmpg123-dev libfftw3-dev librtlsdr-dev libusb-1.0-0-dev mesa-common-dev libglu1-mesa-dev libpulse-dev libsoapysdr-dev libairspy-dev libmp3lame-dev

sudo apt-get install cmake

cd welle.io
mkdir build
cd build

cmake .. -DRTLSDR=1 -DSOAPYSDR=1 -DBUILD_WELLE_IO=OFF 
make 
make install 

sudo apt-get install jq
