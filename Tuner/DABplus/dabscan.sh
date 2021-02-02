#!/bin/bash
DABChannels=(5A 5B 5C 5D 6A 6B 6C 6D 7A 7B 7C 7D 8A 8B 8C 8D 9A 9B 9C 9D 10A 10B 10C 10D 11A 11B 11C 11D 12A 12B 12C 12D 13A 13B 13C 13D 13E 13F)

rm *.servicelinks
rm *.servicenames
sudo killall welle-cli

for item in ${DABChannels[*]}
do
   printf "searching channel %s\n" $item
   welle-cli -c $item -w 2345 &
   sleep 10

   curl -L "127.0.0.1:2345/mux.json" | jq -r .services[].label.label >> $item.servicenames
   curl -L "127.0.0.1:2345/mux.json" | jq -r .services[].url_mp3 >> $item.servicelinks
   sudo killall welle-cli
done

./dab2lists.sh
