/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


static void clearLine() {
	uartPutc('\r');
	for (uint8_t c = 0; c <= CMD_LENGTH_LIMIT + 2; c++)
		uartPutc(' ');
	uartPuts("\r$ ");
}


void getConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_read_block(param, offset, length);
}


void initCLI(uint8_t *buffer) {
	uint8_t i = 0;
	uartPuts("$ ");

	while (1) {
		buffer[i] = uartGetc();
		uartPutc(buffer[i]);

		// Handle backspace.
		if (buffer[i] == '\b') {
			if (i > 0) {
				i--;
				buffer[i] = '\0';
				clearLine();
				uartPuts(buffer);
				continue;
			}

			i = 0;
			clearLine();
			continue;
		}

		// Parse current command.
		if (buffer[i] == '\r') {
			buffer[i] = '\0';

			// Echo last input (test).
			buffer[i] = '\0';
			if (buffer[0] != '\0') uartPuts("\r\n");
			uartPuts(buffer);

			uartPuts("\r\n$ ");
			i = 0;
			continue;
		}


		i++;
		if (i > CMD_LENGTH_LIMIT) {
			clearLine();
			i = 0;
		}
	}
}


void setConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_update_block(param, offset, length);
}
