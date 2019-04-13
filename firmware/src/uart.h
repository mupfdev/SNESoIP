/* uart.h -*-c-*-
 * A minimalistic UART interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef UART_h
#define UART_h

#ifndef BAUD
#define BAUD     57600UL
#endif
#ifndef F_CPU
#define F_CPU    16000000UL
#endif
#define UBRR_VAL ((F_CPU) / (BAUD * 16UL) - 1)


#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>


#define PUTS_P(s) uartPuts_P(PSTR(s))


void    initUART(void);
uint8_t uartGetc(void);
void    uartPrintArray(unsigned char *array, uint8_t size, uint8_t base, char delimiter);
void    uartPutc(unsigned char c);
void    uartPuts(char *s);
void    uartPuts_P (const char *s);


#endif
