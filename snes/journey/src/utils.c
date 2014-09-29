/* utils.c -*-c-*-
 * Various little helpers.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "utils.h"


void delay(int delay) {
	for (snes_vblank_count = 0; snes_vblank_count < 100; WaitForVBlank())
		spcProcess();
}
