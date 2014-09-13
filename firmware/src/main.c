/* main.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "main.h"


int main(void) {
	INIT_BUFFER;
	snesIO  port0       = 0xffff;
	uint8_t tcpPortLow  = 0;
	uint8_t tcpPortHigh = 0;
	uint8_t tmp[64];
	uint8_t *p          = 0;

	getConfigParam(&tcpPortLow,  SERVER_PORT_L, 1);
	getConfigParam(&tcpPortHigh, SERVER_PORT_H, 1);
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


	// Say a friendly HELO and initiate server login.
	ledOnGreen();
	// TODO.


	while (1) {
		uint16_t received;
		uint16_t datp;
		received = enc28j60PacketReceive(BUFFER_SIZE, buffer);


		// Do something while no packet in queue.
		if (received == 0) {
			port0 = recvInput();


			//sendOutput(port0, port0);
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
		IF_PING() {
			DEBUG_ONLY(PUTS_P("Pong\r\n"););

			make_echo_reply_from_request(buffer, received);
			continue;
		}


		// Process incoming TCP data.
		IF_TCP(tcpPortLow, tcpPortHigh) {
			if (buffer[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
				make_tcp_synack_from_syn(buffer);
				continue;
			}

			if (buffer[TCP_FLAGS_P] & TCP_FLAGS_ACK_V) {
				datp = get_tcp_data_len(buffer);

				if (datp == 0) { // No data, just ack.
					if (buffer[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
						make_tcp_ack_from_any(buffer, 0, 0);
					continue;
				}

				if (strncmp("HELO ", (char *)&(buffer[datp]), 5) != 0) {
					DEBUG_ONLY(PUTS_P("HELO\r\n"););
					make_tcp_ack_from_any(buffer, 0, 0);
					make_tcp_ack_with_data_noflags(buffer, received);
					continue;
				}
				continue;
			}
		}


	}
	return (0);
}
