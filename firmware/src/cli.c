/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


static void clearLine() {
	uartPutc('\r');
	for (uint8_t c = 0; c <= INPUT_MAX_LENGTH + 2; c++)
		uartPutc(' ');
	uartPuts("\r$ ");
}


static uint8_t execCommand(uint8_t *command, uint8_t *param) {
	if (strcmp(command, "echo") == 0) {
		uartPuts(param);
		return 0;
	}

	uartPuts(command);
	uartPuts(": command not found.");
	return 0;
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

		// Delete character..
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

		// Send command.
		if (buffer[i] == '\r') {
			buffer[i] = '\0';

			// Parse current command.
			buffer[i] = '\0';

			if (buffer[0] != '\0') {
				uartPuts("\r\n");

				uint8_t tmp;
				for (tmp = 0; tmp < i; tmp++)
					if (buffer[tmp] == ' ') {
						buffer[tmp] = '\0';
						break;
					}
				tmp++;
				execCommand(buffer, buffer + tmp);
			}

			uartPuts("\r\n$ ");
			i = 0;
			continue;
		}

		// Max input length reached.
		i++;
		if (i > INPUT_MAX_LENGTH) {
			clearLine();
			i = 0;
		}

	}
}


void setConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_update_block(param, offset, length);
}
