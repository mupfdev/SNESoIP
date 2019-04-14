// gpr.c -*-c-*-
// Atmel application note AVR313.


#include <inttypes.h>
#include "gpr.h"


void printHexByte(uint8_t i) {
	uint8_t h, l;

	// High nibble.
	h = i & 0xF0;
	h = h >> 4;
	h = h + '0';

	if (h > '9')
		h = h + 7;

	// Low nibble.
	l = (i & 0x0F)+'0';
	if (l > '9')
		l = l + 7;

	putchar('0');
	putchar('x');
	putchar(h);
	putchar(l);
}
