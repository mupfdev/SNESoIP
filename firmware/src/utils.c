/* utils.c -*-c-*-
 * Various little helpers.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "utils.h"


// No string validation!
uint8_t hextoi(uint8_t *s) {
	uint8_t val;

	if (s[0] > 0x39) s[0] -= 7;
	val = (s[0] & 0xf) * 16;

	if (s[1] > 0x39) s[1] -= 7;
	val = val + (s[1] & 0xf);

	return val;
}


uint32_t strtoint(char* str) {
	char* temp = str;
	uint32_t n = 0;
	while (*temp != '\0') {
		n++;
		temp++;
	}

	return strntoint(str, n);
}


uint32_t strntoint(char* str, int n) {
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
