/* config.h -*-c-*-
 * SNESoIP configuration handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CONFIG_h
#define CONFIG_h

#include <avr/eeprom.h>
#include <string.h>


#define CID            0x03 // Own unique device ID.
#define BufferSize     500  // Do NOT change this value.


// Used by io.c
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


#define Port1ClockDDR  DDRD
#define Port1ClockPORT PORTD
#define Port1ClockPIN  PIND
#define Port1Clock     PD2

#define Port1LatchDDR  DDRD
#define Port1LatchPORT PORTD
#define Port1Latch     PD1

#define Port1DataDDR   DDRD
#define Port1DataPORT  PORTD
#define Port1Data      PD0


#define LEDgreenDDR    DDRC
#define LEDgreenPORT   PORTC
#define LEDgreen       PC0

#define LEDredDDR      DDRC
#define LEDredPORT     PORTC
#define LEDred         PC1


// Used by net/enc28j60.c
#define ENC28J60_CONTROL_PORT PORTB
#define ENC28J60_CONTROL_DDR  DDRB
#define ENC28J60_CONTROL_CS   PORTB2
#define ENC28J60_CONTROL_SO   PORTB4
#define ENC28J60_CONTROL_SI   PORTB3
#define ENC28J60_CONTROL_SCK  PORTB5


//
static char   *serverVHost;
static uint8_t p2cid;
static uint8_t mymac[6]    = { 0x00, 0x09, 0xbf, 0x49, 0x44, CID };


// Will be provided by DHCP or DNS lookup.
static uint8_t gwmac[6];
static uint8_t myip[4];
static uint8_t gwip[4];
static uint8_t serverip[4];
static uint8_t netmask[4];


void readConfig();
void writeConfig(char *config);


#endif
