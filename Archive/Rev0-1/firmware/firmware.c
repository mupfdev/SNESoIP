/* firmware.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "firmware.h"


int main(void) {
	static uint8_t buffer[BufferSize + 1];
	int8_t   i;
	uint8_t  message[4] = { 0, 0, CID, P2CID };
	uint16_t received;
	snesIO   port0 = 0xffff, port1 = 0xffff;


	// Initialise UART.
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	sei();
	uart_puts("\r\nWelcome to the SNESoIP debugging interface.\r\n\n");


	// Initialise basic I/O.
	uart_puts("Initialise basic I/O.\r\n");
	initLed();
	initInput();
	initOutput();


	// Switched mode: B + Y.
	port0 = recvInput();
	if (port0 == 0xfffc) {
		uart_puts("Switched mode enabled.\r\n");
		switchedMode = Enabled;
		ledSignal(5);
	}
	ledOnRed();


	// Initialise network interface.
	uart_puts("Initialise network interface.\n");
	enc28j60Init(mymac);
	_delay_ms(100);
	// Magjack leds configuration, see enc28j60 datasheet, page 11
	// LEDB=yellow LEDA=green
	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
	enc28j60PhyWrite(PHLCON, 0x476);
	_delay_ms(100);


	// Get the initial IP via DHCP and configure network.
	uart_puts("\rGet the initial IP via DHCP and configure network: ");
	init_mac(mymac);
	while (i != 1) {
		received = enc28j60PacketReceive(BufferSize, buffer);
		buffer[BufferSize] = '\0';
		i = packetloop_dhcp_initial_ip_assignment(buffer, received, mymac[5]);
	}
	dhcp_get_my_ip(myip, netmask, gwip);

	uart_puts((char *)itoa(myip[0], buffer, 10)); uart_putc('.');
	uart_puts((char *)itoa(myip[1], buffer, 10)); uart_putc('.');
	uart_puts((char *)itoa(myip[2], buffer, 10)); uart_putc('.');
	uart_puts((char *)itoa(myip[3], buffer, 10));
	uart_puts("\r\n");

	client_ifconfig(myip, netmask);


	// Resolve MAC address from server IP.
	uart_puts("Resolve MAC address from server IP.\r\n");
	if (route_via_gw(serverip)) // Must be routed via gateway.
		get_mac_with_arp(gwip, TransNumGwmac, &arpresolverResultCallback);
	else                        // Server is on local network.
		get_mac_with_arp(serverip, TransNumGwmac, &arpresolverResultCallback);

	while (get_mac_with_arp_wait()) {
		received = enc28j60PacketReceive(BufferSize, buffer);
		// Call packetloop to process ARP reply.
		packetloop_arp_icmp_tcp(buffer, received);
	}


	// Lookup DNS of the server hostname.
	uart_puts("Lookup DNS of the server hostname.\r\n");
	while (dnslkup_haveanswer() != 1) {
		uint16_t tmp;
		received = enc28j60PacketReceive(BufferSize, buffer);
		tmp      = packetloop_arp_icmp_tcp(buffer, received);

		if (received == 0) {
			if (!enc28j60linkup()) continue;
			dnslkup_request(buffer, ServerVHost, gwmac);
			_delay_ms(100);
			continue;
		}

		if (tmp == 0)
			udp_client_check_for_dns_answer(buffer, received);
	}
	dnslkup_get_ip(serverip);


	// Connected.
	uart_puts("Connected.\r\n");
	ledOnGreen();


	while (1) { // Main loop start.
		received = enc28j60PacketReceive(BufferSize, buffer);


		// Do something while no packet in queue.
		if (received == 0) {
			port0 = recvInput();

			// Prepare message and send it to the server.
			for (i = 0; i < 8; i++) { // Lo-Byte.
				char *c = message;

				*c = port0 & (1 << i)
					? *c |  (1 << i)
					: *c & ~(1 << i);
			}

			for (i = 0; i < 8; i++) { // Hi-Byte.
				char *c = message + 1;

				*c = port0 & (1 << i + 8)
					? *c |  (1 << i)
					: *c & ~(1 << i);
			}

			send_udp(buffer, message, sizeof(message), 57351, serverip, 57350, gwmac);


			// Send controller data to SNES.
			if (switchedMode == Disabled)
				sendOutput(port0, port1);
			else
				sendOutput(port1, port0);

			continue;
		}


		// This fixes a bug of yet unknown reason on hardware revision 2.
		/* Answer to ARP requests.
		if (eth_type_is_arp_and_my_ip(buffer, received)) {
			make_arp_answer_from_request(buffer, received);
			continue;
		} */


		// Check if IP packets (ICMP or UDP) are for us.
		if (eth_type_is_ip_and_my_ip(buffer, received) == 0)
			continue;


		// Answer ping with pong.
		if (
			buffer[IP_PROTO_P]  == IP_PROTO_ICMP_V &&
			buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {

			uart_puts("Pong.\r\n");
			make_echo_reply_from_request(buffer, received);
			continue;
		}


		// Listen for UDP packets on port 57351 (0xe007) and process
		// received data.
		if (
			buffer[IP_PROTO_P]       == IP_PROTO_UDP_V &&
			buffer[UDP_DST_PORT_H_P] == 0xe0 &&
			buffer[UDP_DST_PORT_L_P] == 0x07) {

			for (i = 0; i < 8; i++) {
				uint16_t *c = &port1;

				*c = buffer[UDP_DATA_P] & (1 << i)
					? *c |  (1 << i)
					: *c & ~(1 << i);
			}

			for (i = 0; i < 8; i++) {
				uint16_t *c = &port1;

				*c = buffer[UDP_DATA_P + 1] & (1 << i)
					? *c |  (1 << i + 8)
					: *c & ~(1 << i + 8);
			}
		}
	} // Main loop end.


	return (0);
}


void arpresolverResultCallback(uint8_t *ip, uint8_t refnum, uint8_t *mac) {
	if (refnum == TransNumGwmac)
		memcpy(gwmac, mac, 6);
}
