#!/bin/bash

if [ "$1" == "channelsscanned" ]; then
files_exist= ls /home/pi/iRadio/Tuner/DABplus/*.servicelinks | wc -l
echo $files_exist

elif [ "$1" == "stationsfound" ]; then
channels=0
declare -a arrayFile

for file in /home/pi/iRadio/Tuner/DABplus/*.servicelinks
do
  arrayFile=(${arrayFile[*]} "$file")
done

for item in ${arrayFile[*]}
do
  while read -u 3 -r file1; do
   ((channels=channels+1))
  done 3<$item
done

echo $channels
fi
