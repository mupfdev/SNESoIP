/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


static void    clearLine();
static int8_t  execCommand(uint8_t *command, uint8_t *param);
static uint8_t hextoi(uint8_t *s);
static int8_t  ipIsValid (const uint8_t *in);
static int8_t  macIsValid(uint8_t *mac);
static int8_t  parseIP (const uint8_t *in, unsigned *out);
//static void    strToIP(uint8_t *ip, uint8_t *buffer);


static const char cmd_echo[]       PROGMEM = "echo";
static const char cmd_showconfig[] PROGMEM = "showconfig"; // TODO
static const char cmd_quit[]       PROGMEM = "quit";
static const char cmd_mymac[]      PROGMEM = "mymac";
static const char cmd_myip[]       PROGMEM = "myip";       // TODO
static const char cmd_gwip[]       PROGMEM = "gwip";       // TODO
static const char cmd_netmask[]    PROGMEM = "netmask";    // TODO
static const char cmd_dns[]        PROGMEM = "dns";        // TODO
static const char cmd_username[]   PROGMEM = "username";   // TODO
static const char cmd_password[]   PROGMEM = "password";   // TODO
static const char cmd_key[]        PROGMEM = "key";        // TODO
static const char cmd_udpportmin[] PROGMEM = "udpportmin"; // TODO
static const char cmd_udpportmax[] PROGMEM = "udpportmax"; // TODO
static const char cmd_serverport[] PROGMEM = "serverport"; // TODO
static const char cmd_serverhost[] PROGMEM = "serverhost"; // TODO
static const char cmd_wipe[]       PROGMEM = "wipe";


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
				if (i == INVALID_COMMAND) {
					uartPuts(buffer);
					PUTS_P(": invalid command.");
				}
				if (i == INVALID_PARAM) {
					uartPuts(buffer + tmp);
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


void setConfigParam(uint8_t *param, uint8_t offset, uint8_t length) {
	eeprom_update_block(param, offset, length);
}


static void clearLine() {
	uartPutc('\r');
	for (uint8_t c = 0; c <= INPUT_MAX_LENGTH + 2; c++)
		uartPutc(' ');
	PUTS_P("\r$ ");
}


static int8_t execCommand(uint8_t *command, uint8_t *param) {
	uint8_t buffer[128];

	if (COMMAND(cmd_echo)) {
		uartPuts(param);
		return 0;
	}

	if (COMMAND(cmd_quit)) return 1;

	if (COMMAND(cmd_mymac)) {
		if (macIsValid(param) == -1)
			return INVALID_PARAM;

		uartPuts(param);
		buffer[0] = hextoi(param);
		buffer[1] = hextoi(param +  3);
		buffer[2] = hextoi(param +  6);
		buffer[3] = hextoi(param +  9);
		buffer[4] = hextoi(param + 12);
		buffer[5] = hextoi(param + 15);
		setConfigParam(buffer, MYMAC, MYMAC_LEN);
		return 0;
	}

	if (COMMAND(cmd_myip)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		uartPuts(param);
		//strToIP(param, buffer);
		PUTS_P("\r\n");
		uartPrintArray(buffer, 4, 10, '.');
		//setConfigParam(buffer, MYIP, MYIP_LEN);

		return 0;
	}

	if (COMMAND(cmd_wipe)) {
		for (uint16_t i = 0; i < 1024; i++)
			eeprom_update_byte(i, 0);
		return 0;
	}

	return -1;
}


// Attention: no string validation!
// For pre-validation use macIsValid().
static uint8_t hextoi(uint8_t *s) {
	uint8_t val;

	if (s[0] > 0x39) s[0] -= 7;
	val = (s[0] & 0xf) * 16;

	if (s[1] > 0x39) s[1] -= 7;
	val = val + (s[1] & 0xf);

	return val;
}


static int8_t ipIsValid (const uint8_t *in)  {
	uint8_t  c  = 0;
	unsigned ip = 0;

	for (uint8_t i = 0; in[i] != '\0'; i++)
		if (in[i] == '.') {
			c++;
			if (c > 3) return -1;
			if (in[i + 1] == '.') return -1;
		}

	if (!parseIP (in, &ip))
		return 0;
	else
		return -1;
}


static int8_t macIsValid(uint8_t *mac) {
	if (strlen((char *)mac) != 17) return -1;

	for (uint8_t i = 2; i < 14; i = i + 3)
		if (mac[i] != ':') return -1;

	for (uint8_t i = 0; i < 17; i++) {
		if (mac[i] == ':') i++;
		if (isxdigit(mac[i]) == 0) return -1;
	}

	return 0;
}


static int8_t parseIP (const uint8_t *in, unsigned *out) {
	//unsigned ip, seg, exp;
	//ip = seg = exp = *out = 0;
	unsigned seg, exp;
	seg = exp = *out = 0;
	do {
		if (*in == '.') {
			*out = (*out << 8) + seg;
			seg = 0;
			exp++;

		} else {
			seg = 10 * seg + (*in - 0x30);
			if (*in < 0x30 || *in > 0x39 || seg > 255) return -1;
		}
	} while (*++in != '\0');
	*out = (*out << 8) + seg;
	if (exp != 3) return -1;

	return 0;
}
