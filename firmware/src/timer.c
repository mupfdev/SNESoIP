/* timer.c -*-c-*-
 * Simple CTC mode timer routine.
 *
 * To make use of the timer, simple define:
 *
 * ISR (TIMER1_COMPA_vect) { [your code] }
 *
 * somewhere in your program.
 *
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "timer.h"


void initTimer(uint16_t delay) {
	OCR1A = delay;
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	sei();
}


void setTimer(uint16_t delay) { OCR1A = delay; }
