/* firmware.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "firmware.h"


int main(void) {
	INIT_BUFFER;
	snesIO port0 = 0xffff;


	// Initialise basic I/O.
	initLed();
	ledOnRed();

	initUART();
 	INIT_IO();


	// Command-line interface: B + Y.
	port0 = recvInput();
	if (port0 == 0xfffc) {
		initCLI(buffer);
	}


	// Pre-configure device (for testing purposes only).
	uint8_t testmac[6] = { 0x00, 0x09, 0xBF, 0x02, 0x00, 0x00 };
	setConfigParam(testmac, MYMAC, MYMAC_LEN);


	// Initialise network interface.
	getConfigParam(buffer, MYMAC, MYMAC_LEN);
	initNetwork(buffer);
#if DEBUG
	uartPuts("\r\nMAC: ");
	uartPrintArray(buffer, 6, 16, ':');
	uartPuts("\r\n");
#endif


	// Get the initial IP via DHCP and configure network.
#if DEBUG
	uartPuts("IP: ");
	uartPrintArray(setIPviaDHCP(buffer), 4, 10, '.');
	uartPuts("\r\n");
#else
	setIPviaDHCP(buffer);
#endif


	// Resolve MAC address from server or gateway.
#if DEBUG
	uartPuts("Gateway MAC: ");
	uartPrintArray(resolveMAC(buffer), 6, 16, ':');
	uartPuts("\r\n");
#else
	resolveMAC(buffer);
#endif


	// Perform DNS lookup of server hostname.
#if DEBUG
	uartPuts("Server: ");
	uartPrintArray(dnsLookup(buffer, "snesoip.de"), 4, 10, '.');
	uartPuts("\r\n");
#else
	dnsLookup(buffer, "snesoip");
#endif


	// Connected.
	ledOnGreen();


	while (1) {
		uint16_t received;
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);


		// Do something while no packet in queue.
		if (received == 0) {
			port0 = recvInput();

			sendOutput(port0, port0);
			continue;
		}


		// Answer to ARP requests.
		if (eth_type_is_arp_and_my_ip(buffer, received)) {
			make_arp_answer_from_request(buffer);
			continue;
		}


		// Check if IP packets (ICMP or UDP) are for us.
		if (eth_type_is_ip_and_my_ip(buffer, received) == 0)
			continue;

		// Answer ping with pong.
		if (PING) {
#if DEBUG
			uartPuts("Pong.\r\n");
#endif
			make_echo_reply_from_request(buffer, received);
			continue;
		}
	}


	return (0);
}
