/* network.h -*-c-*-
 * Network handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef NETWORK_h
#define NETWORK_h

#define INIT_BUFFER uint8_t buffer[BUFFER_SIZE + 1]; memset(buffer, 0, BUFFER_SIZE)

#define BUFFER_SIZE     500 // Do NOT change this value.
#define TCP_BUFFER_SIZE  64
#define TRANS_NUM_GWMAC   1


#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "net/dhcp_client.h"
#include "net/dnslkup.h"
#include "net/enc28j60.h"
#include "net/ip_arp_udp_tcp.h"
#include "net/net.h"
#include "utils.h"


uint8_t *dnsLookup(uint8_t *buffer, char *host);
void     initNetwork(uint8_t *mac);
int8_t   ipIsValid (const uint8_t *in);
int8_t   macIsValid(uint8_t *mac);
int8_t   portIsValid(uint8_t *port);
uint8_t *resolveMAC(uint8_t *buffer);
uint8_t *setIPviaDHCP(uint8_t *buffer);
uint8_t *setIPviaStaticIP(uint8_t *ip, uint8_t *nmask, uint8_t *gw);
void     strToIP(uint8_t *ip, uint8_t *dst);


#endif
