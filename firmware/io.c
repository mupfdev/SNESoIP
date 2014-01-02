/* io.c -*-c-*-
 * I/O handler.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "io.h"


void initInput() {
	InputClockDDR  |= (1 << InputClock);
	InputLatchDDR  |= (1 << InputLatch);

	InputDataDDR   &= ~(1 << InputData);
	InputDataPORT  |=  (1 << InputData);

	InputClockPORT |=  (1 << InputClock);

	InputLatchPORT &= ~(1 << InputLatch);
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
	int i;
	snesIO input = 0;

	// Initialise transmission.
	InputLatchPORT |= (1 << InputLatch);
	_delay_us(12);
	InputLatchPORT &= ~(1 << InputLatch);


	for (i = 0; i < 16; i++) {
		_delay_us(6);
		InputClockPORT &= ~(1 << InputClock);

		if (InputDataPIN & (1 << InputData))
			input |= (1 << i);

		_delay_us(6);
		InputClockPORT |= (1 << InputClock);
	}

	return input;
}


void sendOutput(snesIO port0, snesIO port1) {
	int i;

	// Initialise transmission.
	Port0LatchPORT |= (1 << Port0Latch);
	Port1LatchPORT |= (1 << Port1Latch);
	_delay_us(12);
	Port0LatchPORT &= ~(1 << Port0Latch);
	Port1LatchPORT &= ~(1 << Port1Latch);


	for (i = 0; i < 16; i++) {
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


void initLed() {
	LEDgreenDDR |= (1 << LEDgreen);
	LEDredDDR   |= (1 << LEDred);
}


void ledOff() {
	LEDgreenPORT &= ~(1 << LEDgreen);
	LEDredPORT   &= ~(1 << LEDred);
}


void ledOnGreen() {
	LEDredPORT   &= ~(1 << LEDred);
	LEDgreenPORT |=  (1 << LEDgreen);
}


void ledOnRed() {
	LEDgreenPORT &= ~(1 << LEDgreen);
	LEDredPORT   |=  (1 << LEDred);
}


void ledToggleGreen() {
	LEDredPORT   &= ~(1 << LEDred);
	LEDgreenPORT ^= (1 << LEDgreen);
}


void ledToggleRed() {
	LEDgreenPORT   &= ~(1 << LEDgreen);
	LEDredPORT   ^= (1 << LEDred);
}


void ledSignal(uint8_t times) {
	ledOff();

	while (times > 0) {
		ledToggleGreen();
		_delay_ms(100);
		ledToggleGreen();
		ledToggleRed();
		_delay_ms(100);
		ledToggleRed();
		times--;
	}

	ledOff();
}
