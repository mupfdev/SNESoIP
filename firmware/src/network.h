/* network.h -*-c-*-
 * 
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef NETWORK_h
#define NETWORK_h


#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "net/dhcp_client.h"
#include "net/dnslkup.h"
#include "net/enc28j60.h"
#include "net/ip_arp_udp_tcp.h"
#include "net/net.h"


#define PING buffer[IP_PROTO_P] == IP_PROTO_ICMP_V && buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V


#define BUFFER_SIZE     500 // Do NOT change this value.
#define TRANS_NUM_GWMAC   1


void     arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac);
uint8_t *dnsLookup(uint8_t *buffer, char *host);
void     initNetwork(uint8_t *mymac);
uint8_t *resolveMAC(uint8_t *buffer);
uint8_t *setIPviaDHCP(uint8_t *buffer);
// To avoid compiler warning about implicit declaration of function:
uint8_t eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len);
void    make_arp_answer_from_request(uint8_t *buf);
void    make_echo_reply_from_request(uint8_t *buf, uint16_t len);


static uint8_t dns[4];
static uint8_t myip[4];
static uint8_t mymac[6];
static uint8_t gwip[4];
static uint8_t gwmac[6];
static uint8_t serverip[4];
static uint8_t netmask[4];


#endif
