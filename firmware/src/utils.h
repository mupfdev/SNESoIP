/* utils.h -*-c-*-
 * Various little helpers.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef UTILS_H
#define UTILS_H


#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


// hextoi(): no string validation!
uint8_t  hextoi(uint8_t *s);
uint32_t strtoint(char* str);
uint32_t strntoint(char* str, int n);


#endif
