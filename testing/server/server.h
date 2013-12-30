/* server.h -*-c-*-
 * SNESoIP server.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef SERVER_h
#define SERVER_h


#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <getopt.h>
#include <libintl.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "config.h"


#define _(String) gettext(String)


typedef uint16_t snesIO;


void  error(char *msg);
char* uint16_t2bin(uint16_t num);


#endif
