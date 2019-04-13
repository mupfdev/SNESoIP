/* timer.h -*-c-*-
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
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef TIMER_h
#define TIMER_h

#define TIMER_16MHZ_1_SECOND  0x3d08
#define TIMER_16MHZ_2_SECONDS 0x7a11
#define TIMER_16MHZ_3_SECONDS 0xb71a


#include <avr/interrupt.h>


void initTimer(uint16_t delay);
void setTimer(uint16_t delay);


#endif
