/* config.c -*-c-*-
 * SNESoIP configuration handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "config.h"


void readConfig() {
	char  buffer[300];
	char *p2cidPos, *vhostPos;
	int   i = 0, offset;


	while (buffer[i] != '\0') {
		buffer[i] = eeprom_read_byte((uint8_t*)i);
		i++;
	}

	p2cidPos = strstr(buffer, "P2CID");
	if (p2cidPos != NULL) {
		p2cid   = atoi(p2cidPos + 5);
		offset  = (int)p2cidPos - (int)buffer;
		offset -= 6;

	} else
		p2cid = 0;


	vhostPos = strstr(buffer, "VHOST");
	if (vhostPos != NULL) {
		for (i = 0; i <= offset; i++)
			serverVHost[i] = buffer[i + 5];
		serverVHost[offset + 1] = '\0';

	} else
		serverVHost = "snesoip.mupfelofen.de";
}


void writeConfig(char *config) {
	int i = 0;

	if (
		(strstr(config, "P2CID") != NULL) &&
		(strstr(config, "VHOST") != NULL)) {

		while (config[i] != '\0') {
			eeprom_update_byte((uint8_t*)i, config[i]);
			i++;
		}

		eeprom_update_byte((uint8_t*)i + 1, '\0');
	}
}
