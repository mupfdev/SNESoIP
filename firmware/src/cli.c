/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


static void   clearLine();
static int8_t execCommand(uint8_t *command, uint8_t *param);
static int8_t isMacValid(uint8_t *mac);


void getConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_read_block(param, offset, length);
}


void initCLI(uint8_t *buffer) {
	uint8_t i = 0;
	uartPuts("$ ");

	while (1) {
		buffer[i] = uartGetc();
		uartPutc(buffer[i]);

		// Delete last character.
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

		// Execute command.
		if (buffer[i] == '\r') {
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
				if (execCommand(buffer, buffer + tmp) == -1) {
					uartPuts(buffer);
					uartPuts(": invalid command.");
				}
				memset(buffer, 0, INPUT_MAX_LENGTH);
			}

			uartPuts("\r\n$ ");
			i = 0;
			continue;
		}

		// Delete current input if maximum input length reached.
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


static void clearLine() {
	uartPutc('\r');
	for (uint8_t c = 0; c <= INPUT_MAX_LENGTH + 2; c++)
		uartPutc(' ');
	uartPuts("\r$ ");
}


static int8_t execCommand(uint8_t *command, uint8_t *param) {
	if (strcmp(command, "echo") == 0) {
		uartPuts(param);
		uartPuts("\r\n");
		return 0;
	}

	return -1;
}


static int8_t isMacValid(uint8_t *mac) {

	return 0;
}
