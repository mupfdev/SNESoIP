#! /bin/bash

socat -v -T30 \
	UDP-LISTEN:`echo $(awk -F "= " '/^port/ {print $2}' server.conf)`,fork \
	TCP:localhost:`echo $(awk -F "= " '/^gwport/ {print $2}' server.conf)` &
./server
