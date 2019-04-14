# -*-makefile-*-
.PHONY: all clean


all:
	gcc -g -O2 -Wall \
	-lconfig `mysql_config --cflags --libs` \
	-o ./vsnesoip  \
	axbtnmap.c vsnesoip.c login.c \
	 -lpthread


clean:
	rm ./vsnesoip

