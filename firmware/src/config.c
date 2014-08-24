/* config.c -*-c-*-
 * Configuration interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "config.h"


void getConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_read_block(param, offset, length);
}


void setConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_update_block(param, offset, length);
}
