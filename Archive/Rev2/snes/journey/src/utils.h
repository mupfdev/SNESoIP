/* utils.h -*-c-*-
 * Various little helpers.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef UTILS_h
#define UTILS_h


#include <snes.h>
#include <stdint.h>


void delay(u16 vblanks);
void textFader(uint16_t vblanks, uint16_t x, uint16_t y, char *string);


#endif
