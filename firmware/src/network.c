/* network.c -*-c-*-
 * Network handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "network.h"


static uint8_t myip[4];
static uint8_t mymac[6];
static uint8_t gwip[4];
static uint8_t gwmac[6];
static uint8_t serverip[4];
static uint8_t netmask[4];


static void     arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac);
static int8_t   parseIP(const uint8_t *in, unsigned *out);


uint8_t *dnsLookup(uint8_t *buffer, char *host) {
	uint16_t received;

	// Check if the host is already a valid IP address.
	if (ipIsValid((uint8_t *)host) == 0) {
		strToIP((uint8_t *)host, serverip);
		return serverip;
	}

	while (dnslkup_haveanswer() != 1) {
		uint16_t tmp;
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		tmp      = packetloop_arp_icmp_tcp(buffer, received);

		if (received == 0) {
			if (!enc28j60linkup()) continue;
			dnslkup_request(buffer, host, gwmac);
			_delay_ms(100);
			continue;
		}

		if (tmp == 0)
			udp_client_check_for_dns_answer(buffer, received);
	}

	dnslkup_get_ip(serverip);
	return serverip;
}


void initNetwork(uint8_t *mac) {
	memcpy(mymac, mac, 6);

	enc28j60Init(mymac);
	_delay_ms(100);
	// Magjack leds configuration, see enc28j60 datasheet, page 11
	// LEDB=yellow LEDA=green
	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON, 0x476);
	_delay_ms(100);
	init_mac(mymac);
}


int8_t ipIsValid (const uint8_t *in)  {
	uint8_t  c  = 0;
	unsigned ip = 0;

	for (uint8_t i = 0; in[i] != '\0'; i++)
		if (in[i] == '.') {
			c++;
			if (c > 3) return -1;
			if (in[i + 1] == '.') return -1;
		}

	if (!parseIP (in, &ip))
		return 0;
	else
		return -1;
}


int8_t macIsValid(uint8_t *mac) {
	if (strlen((char *)mac) != 17) return -1;

	for (uint8_t i = 2; i < 14; i = i + 3)
		if (mac[i] != ':') return -1;

	for (uint8_t i = 0; i < 17; i++) {
		if (mac[i] == ':') i++;
		if (isxdigit(mac[i]) == 0) return -1;
	}

	return 0;
}


int8_t portIsValid(uint8_t *port) {
	if (port[0] == '\0') return -1;

	if (strtoint((char *)port) > 65535)
		return -1;

	return 0;
}


#include "uart.h" //DEBUG

uint8_t *resolveMAC(uint8_t *buffer) {
	uint16_t received;

	get_mac_with_arp(gwip, TRANS_NUM_GWMAC, &arpresolverResultCallback);

	while (get_mac_with_arp_wait()) {
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		// Call packetloop to process ARP reply.
		packetloop_arp_icmp_tcp(buffer, received);
	}

	return gwmac;
}


uint8_t *setIPviaDHCP(uint8_t *buffer) {
	int8_t   i = 0;
	uint16_t received;

	while (i != 1) {
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		buffer[BUFFER_SIZE] = '\0';
		i = packetloop_dhcp_initial_ip_assignment(buffer, received, mymac[5]);
	}
	dhcp_get_my_ip(myip, netmask, gwip);
	client_ifconfig(myip, netmask);

	return myip;
}


uint8_t *setIPviaStaticIP(uint8_t *ip, uint8_t *nmask, uint8_t *gw) {
	memcpy(myip, ip, 4);
	memcpy(netmask, nmask, 4);
	memcpy(gwip, gw, 4);

	client_ifconfig(myip, netmask);

	return myip;
}


// Attention: no string validation!
// For pre-validation use ipIsValid().
void strToIP(uint8_t *ip, uint8_t *dst) {
	uint8_t i = 0;
	char   *tmp;

	tmp = strtok((char *)ip, ".");

	while (tmp != NULL) {
		dst[i] = atoi(tmp);
		tmp = strtok(NULL, ".");
		i++;
	}
}


static void arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac) {
	ip = ip;
	if (refnum == TRANS_NUM_GWMAC)
		memcpy(gwmac, mac, 6);
}


static int8_t parseIP(const uint8_t *in, unsigned *out) {
	unsigned seg, exp;
	seg = exp = *out = 0;
	do {
		if (*in == '.') {
			*out = (*out << 8) + seg;
			seg = 0;
			exp++;

		} else {
			seg = 10 * seg + (*in - 0x30);
			if (*in < 0x30 || *in > 0x39 || seg > 255) return -1;
		}
	} while (*++in != '\0');
	*out = (*out << 8) + seg;
	if (exp != 3) return -1;

	return 0;
}
