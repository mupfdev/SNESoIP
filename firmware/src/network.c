/* network.c -*-c-*-
 * Network handler.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "network.h"


static uint8_t  myip[4];
static uint8_t  mymac[6];
static uint8_t  gwip[4];
static uint8_t  gwmac[6];
static uint8_t  serverip[4];
static uint8_t  netmask[4];

static uint8_t *bufp;
static char     tcpBuffer[TCP_BUFFER_SIZE];
static uint8_t  tcpFD = 0;


static void     arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac);
static uint16_t tcpDatafillCallback(uint8_t fd);
static uint8_t  tcpResultCallback(uint8_t fd, uint8_t statuscode, uint16_t datapos, uint16_t datalen);


uint8_t *dnsLookup(uint8_t *buffer, char *host) {
	uint16_t received;

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


void initTCPclient(uint8_t *buffer) {
	bufp = buffer;
	memset(tcpBuffer, 0, TCP_BUFFER_SIZE);
}


uint8_t *resolveMAC(uint8_t *buffer) {
	uint16_t received;

	if (route_via_gw(serverip)) // Must be routed via gateway.
		get_mac_with_arp(gwip, TRANS_NUM_GWMAC, &arpresolverResultCallback);
	else                        // Server is on local network.
		get_mac_with_arp(serverip, TRANS_NUM_GWMAC, &arpresolverResultCallback);

	while (get_mac_with_arp_wait()) {
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		// Call packetloop to process ARP reply.
		packetloop_arp_icmp_tcp(buffer, received);
	}

	return gwmac;
}


void sendTCPrequest(char *data, uint16_t port) {
	memcpy(tcpBuffer, data, TCP_BUFFER_SIZE);
	tcpFD = client_tcp_req(&tcpResultCallback,&tcpDatafillCallback, port, serverip, gwmac);
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


static void arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac) {
	ip = ip;
	if (refnum == TRANS_NUM_GWMAC)
		memcpy(gwmac, mac, 6);
}


static uint16_t tcpDatafillCallback(uint8_t fd) {
	uint16_t len = 0;;

	if (fd == tcpFD) {
		len = fill_tcp_data(bufp, 0, tcpBuffer);
		return len;
	}

	return 0;
}


static uint8_t tcpResultCallback(uint8_t fd, uint8_t statuscode, uint16_t datapos, uint16_t datalen) {
	fd         = fd;
	statuscode = statuscode;
	datapos    = datapos;
	datalen    = datalen;

	return 0;
}
