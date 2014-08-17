/* duoled.h -*-c-*-
 * 
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef DUOLED_h
#define DUOLED_h

#include <avr/io.h>
#include <util/delay.h>


#define LEDgreenDDR   DDRC
#define LEDgreenPORT  PORTC
#define LEDgreen      PC0

#define LEDredDDR     DDRC
#define LEDredPORT    PORTC
#define LEDred        PC1


void initLed(void);
void ledOff(void);
void ledOnGreen(void);
void ledOnRed(void);
void ledToggleGreen(void);
void ledToggleRed(void);
void ledSignal(uint8_t times);


#endif
