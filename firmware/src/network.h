/* network.h -*-c-*-
 * Network handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef NETWORK_h
#define NETWORK_h

#define IF_PING() if(buffer[IP_PROTO_P] == IP_PROTO_ICMP_V && buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
#define IF_TCP(lowp, highp) if (buffer[IP_PROTO_P] == IP_PROTO_TCP_V && buffer[TCP_DST_PORT_H_P] == highp && buffer[TCP_DST_PORT_L_P] == lowp)
#define INIT_BUFFER uint8_t buffer[BUFFER_SIZE + 1]; memset(buffer, 0, BUFFER_SIZE)

#define BUFFER_SIZE     500 // Do NOT change this value.
#define TRANS_NUM_GWMAC   1


#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "net/dhcp_client.h"
#include "net/dnslkup.h"
#include "net/enc28j60.h"
#include "net/ip_arp_udp_tcp.h"
#include "net/net.h"


uint8_t *dnsLookup(uint8_t *buffer, char *host);
void     initNetwork(uint8_t *mac);
uint8_t *resolveMAC(uint8_t *buffer);
uint8_t *setIPviaDHCP(uint8_t *buffer);
// To avoid compiler warning about implicit declaration of function:
uint8_t  eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len);
uint16_t get_tcp_data_len(uint8_t *buf);
void     make_arp_answer_from_request(uint8_t *buf);
void     make_echo_reply_from_request(uint8_t *buf, uint16_t len);
void     make_tcp_ack_from_any(uint8_t *buf, int16_t datlentoack, uint8_t addflags);
void     make_tcp_ack_with_data_noflags(uint8_t *buf,uint16_t dlen);
void     make_tcp_synack_from_syn(uint8_t *buf);


#endif
