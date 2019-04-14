/* snesio.h -*-c-*-
 * SNES I/O handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef IO_h
#define IO_h

#define INIT_IO() initInput(); initIO(); initOutput()

#define InputClockDDR  DDRD
#define InputClockPORT PORTD
#define InputClock     PD5

#define InputLatchDDR  DDRD
#define InputLatchPORT PORTD
#define InputLatch     PD6

#define InputDataDDR   DDRD
#define InputDataPORT  PORTD
#define InputDataPIN   PIND
#define InputData      PD7


#define Port0ClockDDR  DDRC
#define Port0ClockPORT PORTC
#define Port0ClockPIN  PINC
#define Port0Clock     PC2

#define Port0LatchDDR  DDRC
#define Port0LatchPORT PORTC
#define Port0Latch     PC3

#define Port0DataDDR   DDRC
#define Port0DataPORT  PORTC
#define Port0Data      PC4

#define Port0WRIO_DDR  DDRB
#define Port0WRIO_PORT PORTB
#define Port0WRIO_PIN  PINB
#define Port0WRIO      PB1


#define Port1ClockDDR  DDRD
#define Port1ClockPORT PORTD
#define Port1ClockPIN  PIND
#define Port1Clock     PD4

#define Port1LatchDDR  DDRD
#define Port1LatchPORT PORTD
#define Port1Latch     PD3

#define Port1DataDDR   DDRD
#define Port1DataPORT  PORTD
#define Port1Data      PD2

#define Port1WRIO_DDR  DDRB
#define Port1WRIO_PORT PORTB
#define Port1WRIO_PIN  PINB
#define Port1WRIO      PB0


#include <avr/io.h>
#include <util/delay.h>


typedef uint16_t snesIO;


void    initInput(void);
void    initIO();
void    initOutput(void);

snesIO  recvInput(void);
uint8_t recvIO(uint8_t port);
void    sendOutput(snesIO port0, snesIO port1);


#endif
