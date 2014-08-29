/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


void getConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_read_block(param, offset, length);
}


int8_t initCLI(uint8_t *buffer) {
	int8_t i = 0;
	PUTS_P("\r\n$ ");

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
				PUTS_P("\r\n");

				uint8_t tmp;
				for (tmp = 0; tmp < i; tmp++)
					if (buffer[tmp] == ' ') {
						buffer[tmp] = '\0';
						break;
					}
				tmp++;

				i = execCommand(buffer, buffer + tmp);
				if (i == -1) {
					uartPuts(buffer);
					PUTS_P(": invalid command.");
				}
				if (i == 1) return 0; // Quit.

				memset(buffer, 0, INPUT_MAX_LENGTH);
			}

			PUTS_P("\r\n$ ");
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
	PUTS_P("\r$ ");
}


static const uint8_t cmd_echo[]       PROGMEM = "echo";
static const uint8_t cmd_help[]       PROGMEM = "help";       // TODO
static const uint8_t cmd_setmymac[]   PROGMEM = "setmymac";   // TODO
static const uint8_t cmd_setgwip[]    PROGMEM = "setgwip";    // TODO
static const uint8_t cmd_setmyip[]    PROGMEM = "setmyip";    // TODO
static const uint8_t cmd_setdns[]     PROGMEM = "setdns";     // TODO
static const uint8_t cmd_showconfig[] PROGMEM = "showconfig"; // TODO
static const uint8_t cmd_toggledhcp[] PROGMEM = "toggledhcp"; // TODO
static const uint8_t cmd_toggledns[]  PROGMEM = "toggledns";  // TODO
static const uint8_t cmd_quit[]       PROGMEM = "quit";


static int8_t execCommand(uint8_t *command, uint8_t *param) {
	if (strcmp_PF(command, pgm_get_far_address(cmd_echo)) == 0) {
		uartPuts(param);
		return 0;
	}

	if (strcmp_PF(command, pgm_get_far_address(cmd_quit)) == 0)
		return 1;

	return -1;
}


static int8_t isMacValid(uint8_t *mac) {
	// TODO.
	return 0;
}
