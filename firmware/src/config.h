/* config.h -*-c-*-
 * Configuration interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CONFIG_h
#define CONFIG_h

#include <avr/eeprom.h>


#define FLAGS            0x00
#define MYMAC            0x04
#define MYIP             0x0A
#define GWIP             0x0E
#define NETMASK          0x12
#define DNS              0x16
#define SERVERIP         0x1A
#define USERNAME         0x1E
#define PASSWORD         0x3E
#define KEY              0x5E
#define UDP_PORT_MIN     0x68
#define UDP_PORT_MAX     0x6A
#define SERVERHOST       0x6C

#define FLAGS_LEN        0x04
#define MYMAC_LEN        0x06
#define MYIP_LEN         0x04
#define GWIP_LEN         0x04
#define NETMASK_LEN      0x04
#define DNS_LEN          0x04
#define SERVERIP_LEN     0x04
#define USERNAME_LEN     0x20
#define PASSWORD_LEN     0x20
#define KEY_LEN          0x0A
#define UDP_PORT_MIN_LEN 0x02
#define UDP_PORT_MAX_LEN 0x02
#define SERVERHOST_LEN   0x80


void    getConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
void    halt();
uint8_t ipIsValid(uint8_t *ip);
uint8_t macIsValid(uint8_t *mac);



#endif
