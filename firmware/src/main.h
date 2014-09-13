/* main.h -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef MAIN_h
#define MAIN_h

#if (DEBUG)
  #define DEBUG_ONLY(x...) x
  #define BEGIN_DEBUG_ONLY if (1) {
  #define END_DEBUG_ONLY }
#else
  #define DEBUG_ONLY(x) {}
  #define BEGIN_DEBUG_ONLY if (0) {
  #define END_DEBUG_ONLY }
#endif


#include "cli.h"
#include "duoled.h"
#include "network.h"
#include "snesio.h"
#include "trivium.h"
#include "uart.h"


void pingCallback(uint8_t *ip);


#endif
