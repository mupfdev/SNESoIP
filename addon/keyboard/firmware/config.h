/* config.h -*-c-*-
 * SNES keyboard configuration.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CONFIG_h
#define CONFIG_h

#include <avr/io.h>
#include <util/delay.h>


// Used by io.c
#define Port0ClockDDR   DDRB
#define Port0ClockPORT  PORTB
#define Port0ClockPIN   PINB
#define Port0Clock      PB1

#define Port0LatchDDR   DDRB
#define Port0LatchPORT  PORTB
#define Port0Latch      PB2

#define Port0DataDDR    DDRB
#define Port0DataPORT   PORTB
#define Port0Data       PB3


// Used by kb.c
#define KeyboardDataPIN PINB
#define KeyboardData    PB4


#endif
