/* io.c -*-c-*-
 * (downgraded) I/O handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "io.h"


void initOutput() {
	Port0ClockDDR  &= ~(1 << Port0Clock);
	Port0LatchDDR  |=  (1 << Port0Latch);

	Port0DataDDR  |=  (1 << Port0Data);
	Port0DataPORT |=  (1 << Port0Data);

	Port0ClockPORT |= (1 << Port0Clock);

	Port0LatchPORT &= ~(1 << Port0Latch);
}


void sendOutput(snesIO port0) {
	int i;

	// Initialise transmission.
	Port0LatchPORT |= (1 << Port0Latch);
	_delay_us(12);
	Port0LatchPORT &= ~(1 << Port0Latch);


	for (i = 0; i < 16; i++) {
		loop_until_bit_is_clear(Port0ClockPIN, Port0Clock);

		if ((port0 & (1 << i)) == 0)
			Port0DataPORT &= ~(1 << Port0Data);
		else
			Port0DataPORT |= (1 << Port0Data);

		loop_until_bit_is_set(Port0ClockPIN, Port0Clock);
	}
}
