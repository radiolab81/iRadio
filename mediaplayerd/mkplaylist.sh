#!/bin/bash

DEVICE=/mnt
FILES=(*.mp3 *.mp4 *.aac *.wav)

rm medialist.m3u

for item in ${FILES[*]}
do
ls ${DEVICE}/$item 2> /dev/null
if [ $? -eq 0 ] ; then
 ls ${DEVICE}/$item >> medialist.m3u
fi
done

exit 0
