#! /bin/bash

socat -v UDP-LISTEN:51000,fork TCP:localhost:`echo $(awk -F "= " '/port/ {print $2}' server.conf)` &
./server
