/* main.c -*-c-*-
 * The Journey of the lonely wanderer.
 * Author: Michael Fitzmayer
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include <snes.h>
#include "soundbank.h"


extern char snesfont;
extern char __SOUNDBANK__;


int main(void) {
	spcBoot();
	spcSetBank(&__SOUNDBANK__);
	spcAllocateSoundRegion(39);
	spcLoad(MOD_LONELY_SOUL);
	spcPlay(0);

	consoleInit();
	consoleInitText(0, 0, &snesfont);
	setMode(BG_MODE1, 0);
	bgSetDisable(1);
	bgSetDisable(2);

	consoleDrawText(10, 10, "The journey");
	setBrightness(0xF);

	while(1) {
		spcProcess();
		WaitForVBlank();
	}

	return 0;
}
