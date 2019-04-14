/* firmware.c -*-c-*-
 * SNES keyboard firmware.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "firmware.h"


int main() {
	uint8_t key;
	snesIO  output;


	initKeyboard();
	initOutput();
	sei();


	while (1) {
		key = getKbChar();
		output = 0xffff - key;

		sendOutput(output);
	}


	return 0;
}
