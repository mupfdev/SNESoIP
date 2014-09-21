/* cli.c -*-c-*-
 * Command-line/config interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


#if (CLI)
#define INPUT_MAX_LENGTH   96
#define INVALID_COMMAND    -1
#define INVALID_PARAM      -2
#define COMMAND(cmd) strcmp_PF((char *)command, pgm_get_far_address(cmd)) == 0


static void   clearLine();
static int8_t execCommand(uint8_t *command, uint8_t *param);
static void   wipeEEPROM();


static const char cmd_quit[]         PROGMEM = "quit";
static const char cmd_wipe[]         PROGMEM = "wipe";
static const char cmd_mymac[]        PROGMEM = "mymac";
static const char cmd_myip[]         PROGMEM = "myip";
static const char cmd_gwip[]         PROGMEM = "gwip";
static const char cmd_netmask[]      PROGMEM = "netmask";
static const char cmd_username[]     PROGMEM = "username";
static const char cmd_password[]     PROGMEM = "password";
static const char cmd_key[]          PROGMEM = "key";
static const char cmd_sourceport[]   PROGMEM = "sourceport";
static const char cmd_serverport[]   PROGMEM = "serverport";
static const char cmd_serverhost[]   PROGMEM = "serverhost";
#endif


void getConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_read_block(param, offset, length);
}


#if (CLI)
int8_t initCLI(uint8_t *buffer) {
	int8_t i = 0;

	PUTS_P("\r\nCommand-line interface.");
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
				uartPuts((char *)buffer);
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
				if (i == INVALID_COMMAND) {
					uartPuts((char *)buffer);
					PUTS_P(": invalid command.");
				}
				if (i == INVALID_PARAM) {
					uartPuts((char *)buffer + tmp);
					PUTS_P(": invalid parameter.");
				}

				if (i == 1) return 0; // Quit.

				memset(buffer, 0, INPUT_MAX_LENGTH);
			}

			PUTS_P("\r\n$ ");
			i = 0;
			continue;
		}

		// Delete current input if maximum input length is reached.
		i++;
		if (i > INPUT_MAX_LENGTH) {
			clearLine();
			i = 0;
		}

	}
}
#endif


void setConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_update_block(param, offset, length);
}




#if (CLI)
static void clearLine() {
	uartPutc('\r');
	for (uint8_t c = 0; c <= INPUT_MAX_LENGTH + 2; c++)
		uartPutc(' ');
	PUTS_P("\r$ ");
}


static int8_t execCommand(uint8_t *command, uint8_t *param) {
	uint8_t  buffer[128];
	uint16_t port = 0;

	// quit:
	if (COMMAND(cmd_quit)) return 1;

	// wipe:
	if (COMMAND(cmd_wipe)) {
		wipeEEPROM();
		return 0;
	}

	// mymac:
	if (COMMAND(cmd_mymac)) {
		if (macIsValid(param) == -1)
			return INVALID_PARAM;

		buffer[0] = hextoi(param);
		buffer[1] = hextoi(param +  3);
		buffer[2] = hextoi(param +  6);
		buffer[3] = hextoi(param +  9);
		buffer[4] = hextoi(param + 12);
		buffer[5] = hextoi(param + 15);
		setConfigParam(buffer, EEPROM_MYMAC, EEPROM_MYMAC_LEN);
		return 0;
	}

	// myip:
	if (COMMAND(cmd_myip)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, EEPROM_MYIP, EEPROM_MYIP_LEN);
		return 0;
	}

	// gwip:
	if (COMMAND(cmd_gwip)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, EEPROM_GWIP, EEPROM_GWIP_LEN);
		return 0;
	}

	// netmask:
	if (COMMAND(cmd_netmask)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, EEPROM_NETMASK, EEPROM_NETMASK_LEN);
		return 0;
	}

	// username:
	if (COMMAND(cmd_username)) {
		if (strlen((const char *)param) > EEPROM_USERNAME_LEN)
			return INVALID_PARAM;

		setConfigParam(param, EEPROM_USERNAME, EEPROM_USERNAME_LEN);
		return 0;
	}

	// password:
	if (COMMAND(cmd_password)) {
		if (strlen((const char *)param) > EEPROM_PASSWORD_LEN)
			return INVALID_PARAM;

		setConfigParam(param, EEPROM_PASSWORD, EEPROM_PASSWORD_LEN);
		return 0;
	}

	// key:
	if (COMMAND(cmd_key)) {
		if (strlen((const char *)param) != EEPROM_KEY_LEN)
			return INVALID_PARAM;

		setConfigParam(param, EEPROM_KEY, EEPROM_KEY_LEN);
		return 0;
	}

	// sourceport:
	if (COMMAND(cmd_sourceport)) {
		if (portIsValid(param) == -1)
			return INVALID_PARAM;

		port = strtoint((char *)param);
		buffer[0] =  port & 0xff;
		buffer[1] = (port >> 8);
		setConfigParam(buffer, EEPROM_SOURCE_PORT, EEPROM_SOURCE_PORT_LEN);
		return 0;
	}

	// serverport:
	if (COMMAND(cmd_serverport)) {
		if (portIsValid(param) == -1)
			return INVALID_PARAM;

		port = strtoint((char *)param);
		buffer[0] =  port & 0xff;
		buffer[1] = (port >> 8);
		setConfigParam(buffer, EEPROM_SERVER_PORT, EEPROM_SERVER_PORT_LEN);
		return 0;
	}

	// serverhost:
	if (COMMAND(cmd_serverhost)) {
		if (strlen((const char *)param) > EEPROM_SERVER_HOST_LEN)
			return INVALID_PARAM;

		setConfigParam(param, EEPROM_SERVER_HOST, EEPROM_SERVER_HOST_LEN);
		return 0;
	}


	return -1;
}


static void wipeEEPROM() {
	for (uint16_t i = 0; i < 1024; i++)
		eeprom_update_byte(i, 0);
}
#endif
