/* duoled.c -*-c-*-
 * DuoLED control functions.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "duoled.h"


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
