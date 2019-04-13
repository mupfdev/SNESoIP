/* main.c -*-c-*-
 * SNESoIP firmware.
 * Copyright (c) 2014 Michael Fitzmayer.  All rights reserved.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "main.h"


static uint8_t loginState = STATE_SEND_HELO;


int main(void) {
	uint8_t buffer[BUFFER_SIZE + 1];
	snesIO  port0 = 0xffff;
	uint8_t tmp1[64];
	uint8_t tmp2[4];

	memset(buffer, 0, BUFFER_SIZE);
	memset(tmp1, 0, 64);
	memset(tmp2, 0, 4);


	// Initialise basic I/O.
	//initTimer(TIMER_16MHZ_3_SECONDS);
	initLed();
	ledOnRed();
	initUART();
 	INIT_IO();


	// Command-line interface: B + Y at boot time.
	port0 = recvInput();
	if (port0 == 0xfffc)
		CLI_ONLY(initCLI(buffer););


	// Initialise network interface.
	getConfigParam(buffer, EEPROM_MYMAC, EEPROM_MYMAC_LEN);
	initNetwork(buffer);

	BEGIN_DEBUG_ONLY;
	PUTS_P("\r\nMAC: ");
	uartPrintArray((unsigned char *)buffer, 6, 16, ':');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Check if a static IP is set (IP is not 0.0.0.0 nor 255.255.255.255).
	// Explanation:
	// 0.0.0.0         : state after 'wipe' command was used.
	// 255.255.255.255 : default ex-factory.
	getConfigParam(tmp1, EEPROM_MYIP, EEPROM_MYIP_LEN);

	uint8_t cnt1 = 0;
	uint8_t cnt2 = 0;

	for (uint8_t i = 0; i < 3; i++) {
		if (tmp1[i] == 0x00) cnt1++;
		if (tmp1[i] == 0xff) cnt2++;
	}


	// Set IP (DHCP or static) and confiugure network.
	uint8_t *myip = 0;

	DEBUG_ONLY(PUTS_P("IP: "););
	if ((cnt1 == 3) || (cnt2 == 3))
		myip = setIPviaDHCP(buffer);
	else {
		getConfigParam(buffer, EEPROM_NETMASK, EEPROM_NETMASK_LEN);
		getConfigParam(tmp2,   EEPROM_GWIP,    EEPROM_GWIP_LEN);
		myip = setIPviaStaticIP(tmp1, buffer, tmp2);
	}
	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)myip, 4, 10, '.');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;

	ledOnGreen();


	// Resolve MAC address from server or gateway.
	DEBUG_ONLY(PUTS_P("Gateway MAC: "););

	uint8_t *gwmac = 0;
	gwmac = resolveMAC(buffer);

	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)gwmac, 6, 16, ':');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Perform DNS lookup of server hostname.
	getConfigParam(tmp1, EEPROM_SERVER_HOST, EEPROM_SERVER_HOST_LEN);

	if (tmp1[0] == '\0') {
		DEBUG_ONLY(PUTS_P("Error: server host not set.\r\n"));
		return -1;
	}
	DEBUG_ONLY(PUTS_P("Server: "));;

	uint8_t *serverip = 0;
	serverip = dnsLookup(buffer, (char *)tmp1);

	BEGIN_DEBUG_ONLY;
	uartPrintArray((unsigned char *)serverip, 4, 10, '.');
	PUTS_P("\r\n");
	END_DEBUG_ONLY;


	// Asign source and server port.
	uint16_t serverPort;
	uint16_t sourcePort;

	getConfigParam(buffer, EEPROM_SOURCE_PORT, EEPROM_SOURCE_PORT_LEN);
	sourcePort = ((uint16_t)buffer[1] << 8) | buffer[0];
	getConfigParam(buffer, EEPROM_SERVER_PORT, EEPROM_SERVER_PORT_LEN);
	serverPort = ((uint16_t)buffer[1] << 8) | buffer[0];


	// -- //
	register_ping_rec_callback(&pingCallback);


	while (1) {
		uint16_t plen;
		uint16_t datp;
		uint8_t  udpPayloadLen;
		uint8_t  udpPayloadBuffer[128];

		plen = enc28j60PacketReceive(BUFFER_SIZE, buffer);
		datp = packetloop_arp_icmp_tcp(buffer, plen);

		// Do something while no TCP packet in queue.
		// (That's the way the TCP/IP stack was designed, even if TCP isn't
		// used here)
		if (datp == 0) {
			port0 = recvInput();

			if (! recvIO(0))
				PUTS_P("0\n");

			// Store payload if UDP data is available.
			if (udpDataReceived(buffer, plen, sourcePort)) {

				udpPayloadLen = buffer[UDP_LEN_L_P] - UDP_HEADER_LEN;
				if (udpPayloadLen > 128) udpPayloadLen = 128;
				memcpy(udpPayloadBuffer, buffer + UDP_DATA_P, udpPayloadLen);
				udpPayloadBuffer[udpPayloadLen] = '\0';

				DEBUG_ONLY(PUTS_P("<- "););
				DEBUG_ONLY(uartPuts((char *)udpPayloadBuffer););
				DEBUG_ONLY(PUTS_P("\r"););

				// Test.
				for (uint8_t i = 0; i <= udpPayloadLen; i++)
					sendOutput(port0, 0xffff - udpPayloadBuffer[i]);
			}

			if (loginState == STATE_SEND_HELO) {
				DEBUG_ONLY(PUTS_P("-> HELO\r\n"););
				send_udp(buffer, "HELO", 4, sourcePort, serverip, serverPort, gwmac);
				loginState = STATE_WAIT_FOR_HELO;
			}

			sendOutput(port0, port0);
			continue;
		}
	}

	return (0);
}


ISR (TIMER1_COMPA_vect) {
	PUTS_P("I'm the timer!\r\n");
}


void pingCallback(uint8_t *ip) {
	ip = ip;

	DEBUG_ONLY(PUTS_P("Pong\r\n"););
}
