/* snesio.c -*-c-*-
 * SNES I/O handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "snesio.h"


void initInput() {
	InputClockDDR  |= (1 << InputClock);
	InputLatchDDR  |= (1 << InputLatch);

	InputDataDDR   &= ~(1 << InputData);
	InputDataPORT  |=  (1 << InputData);

	InputClockPORT |=  (1 << InputClock);

	InputLatchPORT &= ~(1 << InputLatch);
}


void initIO() {
	Port0WRIO_DDR &= (1 << Port0WRIO);
	Port1WRIO_DDR &= (1 << Port1WRIO);
}


void initOutput() {
	Port0ClockDDR  &= ~(1 << Port0Clock);
	Port0LatchDDR  |=  (1 << Port0Latch);

	Port0DataDDR  |=  (1 << Port0Data);
	Port0DataPORT |=  (1 << Port0Data);

	Port0ClockPORT |= (1 << Port0Clock);

	Port0LatchPORT &= ~(1 << Port0Latch);


	Port1ClockDDR  &= ~(1 << Port1Clock);
	Port1LatchDDR  |=  (1 << Port1Latch);

	Port1DataDDR  |=  (1 << Port1Data);
	Port1DataPORT |=  (1 << Port1Data);

	Port1ClockPORT |= (1 << Port1Clock);

	Port1LatchPORT &= ~(1 << Port1Latch);
}


snesIO recvInput() {
	snesIO input = 0;

	// Initialise transmission.
	InputLatchPORT |= (1 << InputLatch);
	_delay_us(12);
	InputLatchPORT &= ~(1 << InputLatch);


	for (int i = 0; i < 16; i++) {
		_delay_us(6);
		InputClockPORT &= ~(1 << InputClock);

		if (InputDataPIN & (1 << InputData))
			input |= (1 << i);

		_delay_us(6);
		InputClockPORT |= (1 << InputClock);
	}

	return input;
}


uint8_t recvIO(uint8_t port) {
	if (port)
		return (Port1WRIO_PIN & (1 << Port1WRIO));

	return (Port0WRIO_PIN & (1 << Port0WRIO));
}


void sendOutput(snesIO port0, snesIO port1) {
	// Initialise transmission.
	Port0LatchPORT |= (1 << Port0Latch);
	Port1LatchPORT |= (1 << Port1Latch);
	_delay_us(12);
	Port0LatchPORT &= ~(1 << Port0Latch);
	Port1LatchPORT &= ~(1 << Port1Latch);


	for (int i = 0; i < 16; i++) {
		loop_until_bit_is_clear(Port0ClockPIN, Port0Clock);
		loop_until_bit_is_clear(Port1ClockPIN, Port1Clock);

		if ((port0 & (1 << i)) == 0)
			Port0DataPORT &= ~(1 << Port0Data);
		else
			Port0DataPORT |= (1 << Port0Data);

		if ((port1 & (1 << i)) == 0)
			Port1DataPORT &= ~(1 << Port1Data);
		else
			Port1DataPORT |= (1 << Port1Data);

		loop_until_bit_is_set(Port0ClockPIN, Port0Clock);
		loop_until_bit_is_set(Port1ClockPIN, Port1Clock);
	}
}
