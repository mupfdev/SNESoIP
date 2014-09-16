/* snesoip.c -*-c-*-
 * Custom SNESoIP functions to extend the tuxgraphics TCP/IP stack by
 * Guido Socher: http://tuxgraphics.org/common/src2/article09051/
 *
 * To use it, simply add:
 *
 * #include "snesoip.c"
 *
 * at the end of ip_arp_udp_tcp.c.
 * Also make sure that you defined TCP_client in ip_config.h.
 *
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "snesoip.h"


uint16_t packetloopSNESoIP(uint8_t *buffer, uint16_t plen) {
	if (plen == 0) {
		if (tcp_client_state == 1) {}
			// SYN.
	}


	// Check if IP packets (ICMP or UDP) are for us.
	if (eth_type_is_ip_and_my_ip(buffer, plen) == 0)
		return(0);


	if (buffer[IP_PROTO_P] == IP_PROTO_ICMP_V && buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {
		if (icmp_callback)
			(*icmp_callback)(&(buffer[IP_SRC_P]));

		// Answer ping with pong.
		make_echo_reply_from_request(buffer, plen);
		return(0);
	}


	// This is an important check to avoid working on the wrong packets:
	if (plen < 54 || buffer[IP_PROTO_P] != IP_PROTO_TCP_V )
		// Smaller than the smallest TCP packet (TCP packet with no options section) or not TCP port.
		return(0);


	if (buffer[TCP_DST_PORT_H_P] == TCPCLIENT_SRC_PORT_H) {



	}


	return 0;
}
