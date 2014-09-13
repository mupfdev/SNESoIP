/* main.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "main.h"


int main(void) {
	INIT_BUFFER;
	uint8_t  loginState  = 0;
	uint8_t *p           = 0;
	snesIO   port0       = 0xffff;
	uint16_t serverPort  = 0;
	uint8_t  tmp[64];

	memset(tmp, 0, 64);


	// Initialise basic I/O.
	initLed();
	ledOnRed();

	initUART();
 	INIT_IO();


	// Command-line interface: B + Y at boot time.
	port0 = recvInput();
	if (port0 == 0xfffc)
		CLI_ONLY(initCLI(buffer););
	getConfigParam((uint8_t *)&serverPort,  SERVER_PORT, SERVER_PORT_LEN);


	// Initialise network interface.
	getConfigParam(buffer, MYMAC, MYMAC_LEN);
	initNetwork(buffer);

	BEGIN_DEBUG_ONLY;
	PUTS_P("\r\nMAC: ");
	uartPrintArray((unsigned char *)buffer, 6, 16, ':');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Get the initial IP via DHCP and configure network.
	DEBUG_ONLY(PUTS_P("IP: "););

	p = setIPviaDHCP(buffer);

	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)p, 4, 10, '.');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Resolve MAC address from server or gateway.
	DEBUG_ONLY(PUTS_P("Gateway MAC: "););

	p = resolveMAC(buffer);

	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)p, 6, 16, ':');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Perform DNS lookup of server hostname.
	getConfigParam(tmp, SERVER_HOST, SERVER_HOST_LEN);
	if (tmp[0] == '\0') {
		DEBUG_ONLY(PUTS_P("Error: server host not set.\r\n"));
		return -1;
	}
	DEBUG_ONLY(PUTS_P("Server: "));;

	p = dnsLookup(buffer, (char *)tmp);

	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)p, 4, 10, '.');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	ledOnGreen();
	www_server_port(serverPort);
	register_ping_rec_callback(&pingCallback);


	loginState = 1; // Test.


	while (1) {
		uint16_t plen;
		uint16_t datp;

		plen = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		datp = packetloop_arp_icmp_tcp(buffer, plen);

		// Do something while no packet in queue.
		if (datp == 0) {
			port0 = recvInput();

			sendOutput(port0, port0);
			continue;
		}


		if (loginState == 1)
			if (strncmp("HELO client:", (char *)&(buffer[datp]), 12) == 0) {
				loginState = 2;

				getConfigParam(tmp, USERNAME, USERNAME_LEN);
				plen = 0;
				plen = fill_tcp_data_p(buffer, plen, PSTR("USER "));
				plen = fill_tcp_data(buffer, plen, (const char *)tmp);

				www_server_reply(buffer, plen);
				continue;
			}


		if (loginState == 2) {
			DEBUG_ONLY(PUTS_P("Logged in.\r\n"););
		}


		// Check if IP packets (ICMP or UDP) are for us.
		if (eth_type_is_ip_and_my_ip(buffer, plen) == 0)
			continue;


		// UDP placeholder.
	}


	return (0);
}


void pingCallback(uint8_t *ip) {
	ip = ip;

	DEBUG_ONLY(PUTS_P("Pong\r\n"););
}
