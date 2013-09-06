/* config.c -*-c-*-
 * SNESoIP configuration utility.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>


void  error(char *msg);


int main(int argc, char *argv[]) {
	struct sockaddr_in serverAddr;
	char    message[300] = { 0 };
	int     i            = 0;
	int     sock         = 0;
	uint8_t p2cid;


	if (argc != 4) {
		puts("SNESoIP configuration utility\n");
		printf("Usage: %s <IP> <ServerVHost> <P2CID>\n", argv[0]);
		return 1;
	}


	if (strlen(argv[2]) > 255) {
		puts("ServerVHost exceeds 255 characters.");
		return 1;
	}
	p2cid = atoi(argv[3]);


	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error("Couldn't create socket: ");
		return 1;
	}


	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_port        = htons(57352);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	message[0] = 'V';
	message[1] = 'H';
	message[2] = 'O';
	message[3] = 'S';
	message[4] = 'T';

	for (i = 0; i < strlen(argv[2]); i++)
		message[i + 5] = argv[2][i];

	i = strlen(message);

	message[i + 0] = 'P';
	message[i + 1] = '2';
	message[i + 2] = 'C';
	message[i + 3] = 'I';
	message[i + 4] = 'D';

	i = strlen(message);

	sprintf(message + i, "%d\0", p2cid);
	printf("%s\n", message); // Debug.


	if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		error("Could not send data: ");
		return 1;
	}


	close(sock);
	return 0;
}


void error(char *msg) {
	fprintf(stderr, "%s%s\n", msg, strerror(errno));
}
