/* snesoip.h -*-c-*-
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


#ifndef SNESOIP_h
#define SNESOIP_h


uint16_t packetloopSNESoIP(uint8_t *buffer, uint16_t plen);


#endif
