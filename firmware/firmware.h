/* firmware.h -*-c-*-
 * SNESoIP firmware.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef FIRMWARE_h
#define FIRMWARE_h


#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "net/dhcp_client.h"
#include "net/dnslkup.h"
#include "net/enc28j60.h"
#include "net/ip_arp_udp_tcp.h"
#include "net/net.h"
#include "config.h"
#include "io.h"


#define TransNumGwmac 1


void arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac);
void reset();


#endif
