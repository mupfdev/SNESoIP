/* cli.h -*-c-*-
 * Command-line/config interface.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CLI_h
#define CLI_h

#define EEPROM_FLAGS           0x00
#define EEPROM_MYMAC           0x01
#define EEPROM_MYIP            0x07
#define EEPROM_GWIP            0x0b
#define EEPROM_NETMASK         0x0f
#define EEPROM_USERNAME        0x13
#define EEPROM_PASSWORD        0x33
#define EEPROM_KEY             0x53
#define EEPROM_SOURCE_PORT     0x5d
#define EEPROM_SERVER_PORT     0x5f
#define EEPROM_SERVER_HOST     0x61

#define EEPROM_FLAGS_LEN       0x01
#define EEPROM_MYMAC_LEN       0x06
#define EEPROM_MYIP_LEN        0x04
#define EEPROM_GWIP_LEN        0x04
#define EEPROM_NETMASK_LEN     0x04
#define EEPROM_USERNAME_LEN    0x20
#define EEPROM_PASSWORD_LEN    0x20
#define EEPROM_KEY_LEN         0x0a
#define EEPROM_SOURCE_PORT_LEN 0x02
#define EEPROM_SERVER_PORT_LEN 0x02
#define EEPROM_SERVER_HOST_LEN 0x40

#if (CLI)
  #define CLI_ONLY(x...) x
#else
  #define CLI_ONLY(x) {}
#endif


#include <avr/eeprom.h>
#include "network.h"
#include "uart.h"
#include "utils.h"


void   getConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
void   setConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
#if (CLI)
  int8_t initCLI(uint8_t *buffer);
#endif


#endif
