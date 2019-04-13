/* uart.c -*-c-*-
 * A minimalistic UART interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "uart.h"


void initUART(void) {
	UBRR0H  = (UBRR_VAL >> 8);
	UBRR0L  =  UBRR_VAL;

	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);
	UCSR0C  = (1 << UCSZ01) | (1 << UCSZ00);
}


uint8_t uartGetc(void) {
	while ((UCSR0A & (1 << RXC0)) == 0);
	return UDR0;
}


void uartPrintArray(unsigned char *array, uint8_t size, uint8_t base, char delimiter) {
	char tmp[7];

	for (int i = 0; i < size; i++) {
		if ( (array[i] <= 0x0F) && (base == 16) )
			uartPutc('0');

		uartPuts(itoa(array[i], tmp, base));
		if (i < size - 1) uartPutc(delimiter);
	}
}


void uartPutc(uint8_t c) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}


void uartPuts(char *s) {
	while (*s) {
		uartPutc(*s);
		s++;
	}
}


void uartPuts_P (const char *s) {
		while (1) {
			uint8_t c = pgm_read_byte(s);
			s++;
			if ('\0' == c)
				break;
			uartPutc(c);
		}
}
