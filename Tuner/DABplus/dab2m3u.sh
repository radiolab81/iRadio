#!/bin/bash
DABChannels=(5A 5B 5C 5D 6A 6B 6C 6D 7A 7B 7C 7D 8A 8B 8C 8D 9A 9B 9C 9D 10A 10B 10C 10D 11A 11B 11C 11D 12A 12B 12C 12D 13A 13B 13C 13D 13E 13F)

rm dab.m3u

for item in ${DABChannels[*]}
do

 while read -u 3 -r file1 && read -u 4 -r file2; do
   echo "#EXTINF:-1,"$item "-" $file1 >> dab.m3u
   echo "http://127.0.0.1:2345/"$file2 >> dab.m3u
 done 3<$item.servicenames 4<$item.servicelinks
done


