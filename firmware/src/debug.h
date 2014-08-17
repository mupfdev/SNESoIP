/* debug.h -*-c-*-
 * Debugging interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef DEBUG_h
#define DEBUG_h


#include <stdlib.h>
#include <avr/interrupt.h>
#include "uart.h"


#define DEBUG_INIT()  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU)); sei()


void printArray(uint8_t array[], int size, int base, char delimiter);


#endif
