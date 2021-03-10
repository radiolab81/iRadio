#!/bin/bash

DEVICE=/dev/sda1

ls "${DEVICE}" 2> /dev/null
if [ $? -eq 0 ] ; then
  #echo "${DEVICE} ist anwesend."
  mount "${DEVICE}" /mnt 2> /dev/null
else
  #echo "${DEVICE} ist nicht anwesend."
  umount "${DEVICE}"  2> /dev/null
fi

exit 0
