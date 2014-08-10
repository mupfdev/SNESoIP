/* config.h -*-c-*-
 * SNESoIP configuration.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CONFIG_h
#define CONFIG_h


#ifndef CID
#define CID            0x00
#endif

#ifndef P2CID
#define P2CID          0x00
#endif

#ifndef ServerVHost
#define ServerVHost    "snesoip.de"
#endif

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
#define Port1Clock     PD4

#define Port1LatchDDR  DDRD
#define Port1LatchPORT PORTD
#define Port1Latch     PD3

#define Port1DataDDR   DDRD
#define Port1DataPORT  PORTD
#define Port1Data      PD2


#define LEDgreenDDR    DDRC
#define LEDgreenPORT   PORTC
#define LEDgreen       PC0

#define LEDredDDR      DDRC
#define LEDredPORT     PORTC
#define LEDred         PC1


// Used by uart.h
#define UART_BAUD_RATE 115200


static uint8_t mymac[6]    = { 0x00, 0x09, 0xbf, 0x02, 0x00, CID };

// Will be provided by DHCP or DNS lookup.
static uint8_t gwmac[6];
static uint8_t myip[4];
static uint8_t gwip[4];
static uint8_t serverip[4];
static uint8_t netmask[4];


#endif
