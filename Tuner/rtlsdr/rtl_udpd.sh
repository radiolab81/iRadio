#!/bin/bash

rtl_fm -f $1 -s 170k -A fast -r 32k -g 100 - | aplay -t raw -r 32000 -c 1 -f S16_LE

