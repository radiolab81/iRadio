#!/bin/bash
DABChannels=(5A 5B 5C 5D 6A 6B 6C 6D 7A 7B 7C 7D 8A 8B 8C 8D 9A 9B 9C 9D 10A 10B 10C 10D 11A 11B 11C 11D 12A 12B 12C 12D 13A 13B 13C 13D 13E 13F)

rm /home/pi/iRadio/Tuner/DABplus/*.list

channels=0

for item in ${DABChannels[*]}
do

 while read -u 3 -r file1 && read -u 4 -r file2; do
   echo $item >> /home/pi/iRadio/Tuner/DABplus/mux.list
   echo $file1 >> /home/pi/iRadio/Tuner/DABplus/label.list
   echo "http://127.0.0.1:2345/"$file2 >> /home/pi/iRadio/Tuner/DABplus/url.list
   ((channels=channels+1))
 done 3</home/pi/iRadio/Tuner/DABplus/$item.servicenames 4</home/pi/iRadio/Tuner/DABplus/$item.servicelinks
done

echo $channels Sender wurden gefunden.

