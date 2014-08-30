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
#define SERVERIP         0x18
#define USERNAME         0x1C
#define PASSWORD         0x3C
#define KEY              0x5C
#define UDP_PORT_MIN     0x66
#define UDP_PORT_MAX     0x68
#define SERVER_PORT      0x6A
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
#define SERVER_PORT_LEN  0x02
#define SERVERHOST_LEN   0x80

#define INPUT_MAX_LENGTH   96

#define COMMAND(cmd)  strcmp_PF(command, pgm_get_far_address(cmd)) == 0
#define SHOW_CMD(cmd) PUTS_P("\r\n"); uartPuts_P(cmd); PUTS_P(":\r\t\t")


#include <avr/eeprom.h>
#include <ctype.h>
#include <string.h>
#include "uart.h"


void   getConfigParam(uint8_t *param, uint8_t offset, uint8_t length);
int8_t initCLI(uint8_t *buffer);
void   setConfigParam(uint8_t *param, uint8_t offset, uint8_t length);

static void   clearLine();
static int8_t execCommand(uint8_t *command, uint8_t *param);
static int8_t isMacValid(uint8_t *mac);


#endif
