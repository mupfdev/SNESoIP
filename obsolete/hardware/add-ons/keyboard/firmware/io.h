/* io.h -*-c-*-
 * (downgraded) I/O handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef IO_h
#define IO_h

#include <avr/io.h>
#include <util/delay.h>
#include "config.h"


typedef uint16_t snesIO;


void initOutput();
void sendOutput(snesIO port0);


#endif
