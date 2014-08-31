/* firmware.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "firmware.h"


int main(void) {
	INIT_BUFFER;
	uint8_t debug = DEBUG;
	snesIO  port0 = 0xffff;


	// Initialise basic I/O.
	initLed();
	ledOnRed();

	initUART();
 	INIT_IO();


	// Wipe EEPROM on first bootup.
	getConfigParam(buffer, FLAGS, FLAGS_LEN);
	if ((buffer[0]) & (1 << (0))) wipeEEPROM();


	// Command-line interface: B + Y at boot time.
	port0 = recvInput();
	if (port0 == 0xfffc) {
		DEBUG_MSG("\r\nCommand-line interface.");
		initCLI(buffer);
	}


	// Initialise network interface.
	getConfigParam(buffer, MYMAC, MYMAC_LEN);
	initNetwork(buffer);
	DEBUG_MSG("\r\nMAC: ");
	uartPrintArray(buffer, 6, 16, ':');
	DEBUG_MSG("\r\n");


	// Get the initial IP via DHCP and configure network.
	DEBUG_MSG("IP: ");
	uartPrintArray(setIPviaDHCP(buffer), 4, 10, '.');
	DEBUG_MSG("\r\n");


	// Resolve MAC address from server or gateway.
	DEBUG_MSG("Gateway MAC: ");
	uartPrintArray(resolveMAC(buffer), 6, 16, ':');
	DEBUG_MSG("\r\n");


	// Perform DNS lookup of server hostname.
	DEBUG_MSG("Server: ");
	uartPrintArray(dnsLookup(buffer, "snesoip.de"), 4, 10, '.');
	DEBUG_MSG("\r\n");


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
			DEBUG_MSG("Pong\r\n");

			make_echo_reply_from_request(buffer, received);
			continue;
		}
	}


	return (0);
}
