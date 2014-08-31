/* cli.c -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "cli.h"


#define INPUT_MAX_LENGTH   96
#define INVALID_COMMAND    -1
#define INVALID_PARAM      -2
#define COMMAND(cmd) strcmp_PF((char *)command, pgm_get_far_address(cmd)) == 0


static void     clearLine();
static int8_t   execCommand(uint8_t *command, uint8_t *param);
static uint8_t  hextoi(uint8_t *s);
static int8_t   ipIsValid(const uint8_t *in);
static int8_t   macIsValid(uint8_t *mac);
static int8_t   parseIP(const uint8_t *in, unsigned *out);
static int8_t   portIsValid(uint8_t *port);
static void     strToIP(uint8_t *ip, uint8_t *dst);
static uint32_t strtoint(char* str);
static uint32_t strntoint(char* str, int n);


static const char cmd_quit[]         PROGMEM = "quit";
static const char cmd_wipe[]         PROGMEM = "wipe";
static const char cmd_mymac[]        PROGMEM = "mymac";
static const char cmd_myip[]         PROGMEM = "myip";
static const char cmd_gwip[]         PROGMEM = "gwip";
static const char cmd_netmask[]      PROGMEM = "netmask";
static const char cmd_dns[]          PROGMEM = "dns";
static const char cmd_username[]     PROGMEM = "username";
static const char cmd_password[]     PROGMEM = "password";
static const char cmd_key[]          PROGMEM = "key";
static const char cmd_udpportmin[]   PROGMEM = "udpportmin";
static const char cmd_udpportmax[]   PROGMEM = "udpportmax";
static const char cmd_serverport[]   PROGMEM = "serverport";
static const char cmd_serverhost[]   PROGMEM = "serverhost";


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


void wipeEEPROM() {
	for (uint16_t i = 0; i < 1024; i++)
		eeprom_update_byte(i, 0);
}


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

	// myip:
	if (COMMAND(cmd_myip)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, MYIP, MYIP_LEN);
		return 0;
	}

	// gwip:
	if (COMMAND(cmd_gwip)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, GWIP, GWIP_LEN);
		return 0;
	}

	// netmask:
	if (COMMAND(cmd_netmask)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, NETMASK, NETMASK_LEN);
		return 0;
	}

	// dns:
	if (COMMAND(cmd_dns)) {
		if (ipIsValid(param) == -1)
			return INVALID_PARAM;

		strToIP(param, buffer);
		setConfigParam(buffer, DNS, DNS_LEN);
		return 0;
	}

	// username:
	if (COMMAND(cmd_username)) {
		if (strlen((const char *)param) > USERNAME_LEN)
			return INVALID_PARAM;

		setConfigParam(param, USERNAME, USERNAME_LEN);
		return 0;
	}

	// password:
	if (COMMAND(cmd_password)) {
		if (strlen((const char *)param) > PASSWORD_LEN)
			return INVALID_PARAM;

		setConfigParam(param, PASSWORD, PASSWORD_LEN);
		return 0;
	}

	// key:
	if (COMMAND(cmd_key)) {
		if (strlen((const char *)param) > KEY_LEN)
			return INVALID_PARAM;

		setConfigParam(param, KEY, KEY_LEN);
		return 0;
	}

	// udpportmin:
	if (COMMAND(cmd_udpportmin)) {
		if (portIsValid(param) == -1)
			return INVALID_PARAM;

		port = strtoint((char *)param);
		buffer[0] = (port >> 8);
		buffer[1] =  port & 0xff;
		setConfigParam(buffer, UDP_PORT_MIN, UDP_PORT_MIN_LEN);
		return 0;
	}

	// udpportmax:
	if (COMMAND(cmd_udpportmax)) {
		if (portIsValid(param) == -1)
			return INVALID_PARAM;

		port = strtoint((char *)param);
		buffer[0] = (port >> 8);
		buffer[1] =  port & 0xff;
		setConfigParam(buffer, UDP_PORT_MAX, UDP_PORT_MAX_LEN);
		return 0;
	}

	// serverport:
	if (COMMAND(cmd_serverport)) {
		if (portIsValid(param) == -1)
			return INVALID_PARAM;

		port = strtoint((char *)param);
		buffer[0] = (port >> 8);
		buffer[1] =  port & 0xff;
		setConfigParam(buffer, SERVER_PORT, SERVER_PORT_LEN);
		return 0;
	}

	// serverhost:
	if (COMMAND(cmd_serverhost)) {
		if (strlen((const char *)param) > SERVER_HOST_LEN)
			return INVALID_PARAM;

		setConfigParam(param, SERVER_HOST, SERVER_HOST_LEN);
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


static int8_t parseIP(const uint8_t *in, unsigned *out) {
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


static int8_t portIsValid(uint8_t *port) {
	if (port[0] == '\0') return -1;

	if (strtoint((char *)port) > 65535)
		return -1;

	return 0;
}


// Attention: no string validation!
// For pre-validation use ipIsValid().
static void strToIP(uint8_t *ip, uint8_t *dst) {
	uint8_t i = 0;
	char   *tmp;

	tmp = strtok((char *)ip, ".");

	while (tmp != NULL) {
		dst[i] = atoi(tmp);
		tmp = strtok(NULL, ".");
		i++;
	}
}


static uint32_t strtoint(char* str) {
	char* temp = str;
	uint32_t n = 0;
	while (*temp != '\0') {
		n++;
		temp++;
	}

	return strntoint(str, n);
}


static uint32_t strntoint(char* str, int n) {
	uint32_t sign = 1;
	uint32_t place = 1;
	uint32_t ret = 0;

	int i;
	for (i = n-1; i >= 0; i--, place *= 10) {
		int c = str[i];
		switch (c) {

			case 45:
				if (i == 0) sign = -1;
				else return -1;
				break;
			default:
				if (c >= 48 && c <= 57) ret += (c - 48) * place;
				else return -1;
		}
	}

	return sign * ret;
}
