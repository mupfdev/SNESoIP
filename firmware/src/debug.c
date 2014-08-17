/* debug.c -*-c-*-
 * Debugging interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "debug.h"


void printArray(uint8_t array[], int size, int base, char delimiter) {
	char tmp[7];

	for (int i = 0; i < size; i++) {
		uart_puts((char *)itoa(array[i], tmp, base));
		if (i < size - 1) uart_putc(delimiter);
	}
}
