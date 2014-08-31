/* cli.h -*-c-*-
 * Command-line interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CLI_h
#define CLI_h

#define FLAGS            0x00
#define MYMAC            0x02
#define MYIP             0x08
#define GWIP             0x0C
#define NETMASK          0x10
#define DNS              0x14
#define USERNAME         0x18
#define PASSWORD         0x38
#define KEY              0x58
#define UDP_PORT_MIN     0x62
#define UDP_PORT_MAX     0x64
#define SERVER_PORT      0x66
#define SERVER_HOST      0x68

#define FLAGS_LEN        0x04
#define MYMAC_LEN        0x06
#define MYIP_LEN         0x04
#define GWIP_LEN         0x04
#define NETMASK_LEN      0x04
#define DNS_LEN          0x04
#define USERNAME_LEN     0x20
#define PASSWORD_LEN     0x20
#define KEY_LEN          0x0A
#define UDP_PORT_MIN_LEN 0x02
#define UDP_PORT_MAX_LEN 0x02
#define SERVER_PORT_LEN  0x02
#define SERVER_HOST_LEN  0x40


#include <avr/eeprom.h>
#include <ctype.h>
#include <string.h>
#include "uart.h"


void   getConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
int8_t initCLI(uint8_t *buffer);
void   setConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
void   wipeEEPROM();


#endif
