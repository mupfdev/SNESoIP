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
	uartPuts("\r\nWelcome to the SNESoIP debugging interface.\r\n\n");

	uartPuts("Initialise basic I/O.\r\n");
	INIT_IO();


	// Command-line interface: B + Y.
	port0 = recvInput();
	if (port0 == 0xfffc) {
		uartPuts("Command-line interface.\r\n");
		initCLI(buffer);
	}

	// Pre-configure device (for testing purposes only).
	uint8_t testmac[6] = { 0x00, 0x09, 0xBF, 0x02, 0x00, 0x00 };
	setConfigParam(testmac, MYMAC, MYMAC_LEN);


	// Initialise network interface.
	uartPuts("Initialise network interface: ");
	getConfigParam(buffer, MYMAC, MYMAC_LEN);
	initNetwork(buffer);
	uartPrintArray(buffer, 6, 16, ':');
	uartPuts("\r\n");


	// Get the initial IP via DHCP and configure network.
	uartPuts("\rGet the initial IP via DHCP and configure network: ");
	uartPrintArray(setIPviaDHCP(buffer), 4, 10, '.');
	uartPuts("\r\n");


	// Resolve MAC address from server or gateway.
	uartPuts("Resolve MAC address from server or gateway: ");
	uartPrintArray(resolveMAC(buffer), 6, 16, ':');
	uartPuts("\r\n");


	// Perform DNS lookup of server hostname.
	uartPuts("Perform DNS lookup of server hostname: ");
	uartPrintArray(dnsLookup(buffer, "snesoip.de"), 4, 10, '.');
	uartPuts("\r\n");


	// Connected.
	uartPuts("Connected.\r\n");
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
			uartPuts("Pong\r\n");
			make_echo_reply_from_request(buffer, received);
			continue;
		}
	}


	return (0);
}
