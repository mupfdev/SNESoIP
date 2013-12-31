# -*-makefile-*-
.PHONY: all clean


all:
	gcc -std=c99 -lconfig `mysql_config --cflags --libs` -o server \
	config.c server.c

clean:
	rm server
