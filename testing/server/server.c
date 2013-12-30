/* server.c -*-c-*-
 * SNESoIP server.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "server.h"


int main(int argc, char* argv[]) {
	int  opt;
	char *config = "server.cfg";
	MYSQL *dbCon = mysql_init(NULL);


	// Initialise database connection.
	if (! dbCon) {
		fprintf(stderr, "%s\n", mysql_error(dbCon));
		return EXIT_FAILURE;
	}


	// Parse command-line arguments.
	while ((opt = getopt(argc, argv, "c:v")) != -1)
		switch (opt) {
			case 'v':
				printf("SNESoIP server 2.0\n");
				return EXIT_SUCCESS;
			case 'c':
				config = optarg;
				break;
		}


	// Iinitialise configuration file.
	if (initConfig(config) < 0)
		switch (initConfig(config)) {
			case ErrorFileDoesNotExist:
				printf("%s: %s\n", config, _("file does not exist."));
				return EXIT_FAILURE;
			case ErrorMissingHostname:
				printf("%s: %s\n", config, _("hostname is not set."));

			case ErrorMissingUsername:
				printf("%s: %s\n", config, _("username is not set."));

			case ErrorMissingPassword:
				printf("%s: %s\n", config, _("password is not set."));

			case ErrorMissingDatabase:
				printf("%s: %s\n", config, _("database is not set."));

			default:
				return EXIT_FAILURE;
		}


	// Establish database connection.
	if (mysql_real_connect(dbCon, dbHostname, dbUsername, dbPassword,
			NULL, 0, NULL, 0) == NULL) {

		fprintf(stderr, "%s\n", mysql_error(dbCon));
		mysql_close(dbCon);
		return EXIT_FAILURE;
	}


	puts(" _______ _______ _______ _______         _______ ______");
	puts("|     __|    |  |    ___|     __|.-----.|_     _|   __ \\");
	puts("|__     |       |    ___|__     ||  _  | _|   |_|    __/");
	puts("|_______|__|____|_______|_______||_____||_______|___|   Server\n");
	puts("-c <config>\tload specific config file (defalt: server.cfg).");
	puts("-v \t\tprint version and exit.\n");


	return EXIT_SUCCESS;
}


void error(char *msg) {
	fprintf(stderr, "%s%s\n", msg, strerror(errno));
}


char* uint16_t2bin(uint16_t num) {
	int   bitStrLen = sizeof(uint16_t) * 8 * sizeof(char);
	char* bin       = (char*)malloc(bitStrLen);

	for (int i = (bitStrLen - 1); i >= 0; i--) {
		int k = 1 & num;
		*(bin + i) = ((k == 1) ? '1' : '0');
		num >>= 1;
	}

	bin[16] = '\0';

	return bin;
}


/*
	struct sockaddr_in clientAddr, serverAddr;
	int      len, received, sockfd;
	int8_t   clientID, requestID;
	char     recvBuffer[4];
	char     sendBuffer[2];

	char*    nlpos;
	time_t   ltime;
	char*    timestamp;

	snesIO clientData[256];
	snesIO clientCache = 0xffff;

	for (int i = 0; i < 255; i++)
		clientData[i] = 0xffff;
*/


/*
	sockfd                     = socket(AF_INET, SOCK_DGRAM, 0);
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port        = htons(57350);


	if ((bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) == -1) {
		error("Couldn't bind name to socket: ");
		return 1;

	} else
		printf("Listening on port 57350\n\n");


	while (1) {
		len = sizeof(clientAddr);
		received = recvfrom(sockfd, recvBuffer, 4, 0, (struct sockaddr *)&clientAddr, &len);
		if (received == -1) {
			error("Couldn't receive message: ");
			continue;
		}


		clientID  = recvBuffer[2];
		requestID = recvBuffer[3];

		clientCache  = clientData[clientID];


		// Prepare and store received data.
		for (int i = 0; i < 8; i++) { // Lo-Byte.
			uint16_t *c = clientData + clientID;

			*c = recvBuffer[0] & (1 << i)
				? *c |  (1 << i)
				: *c & ~(1 << i);
		}

		for (int i = 0; i < 8; i++) { // Hi-Byte.
			uint16_t *c = clientData + clientID;

			*c = recvBuffer[1] & (1 << i)
				? *c |  (1 << i + 8)
				: *c & ~(1 << i + 8);
		}

		sendto(sockfd, &clientData[requestID], 2, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));


		// Format timestamp.
		ltime     = time(NULL);
		timestamp = ctime(&ltime);
		nlpos     = strstr(timestamp, "\n");
		strncpy(nlpos, "\0", 1);

		if (clientCache != clientData[clientID])
			printf("%s Client%i: 0b%s\n", timestamp, clientID, uint16_t2bin(clientData[clientID]));

	}

	close(sockfd);
*/
