/* server.h -*-c-*-
 * SNESoIP server.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef SERVER_h
#define SERVER_h


#include <netinet/in.h>
#include <errno.h>
#include <getopt.h>
#include <mysql.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"


#define BufferSize 100


char* uint16_t2bin(uint16_t num);


#endif
