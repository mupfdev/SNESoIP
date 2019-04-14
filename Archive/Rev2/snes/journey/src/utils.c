/* utils.c -*-c-*-
 * Various little helpers.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "utils.h"


void delay(uint16_t vblanks) {
	unsigned short tmp;

	for (snes_vblank_count = 0; snes_vblank_count < vblanks; WaitForVBlank()) {
		spcProcess();
		scanPads();
		tmp = padsCurrent(0);
		if (tmp & KEY_START) break;
	}
}


void textFader(uint16_t vblanks, uint16_t x, uint16_t y, char *string) {
	setBrightness(0x00);

	consoleDrawText(x, y, string);
	setFadeEffect(FADE_IN);
	delay(vblanks - 1);
	setFadeEffect(FADE_OUT);

	uint16_t tmp;
	for (tmp = x; tmp <= 31; tmp++)
		consoleDrawText(tmp, y, " ");

	WaitForVBlank();
	setBrightness(0xff);
}
