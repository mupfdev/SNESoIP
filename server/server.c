/* server.c -*-c-*-
 * SNESoIP server.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "server.h"


int main(int argc, char* argv[]) {
	struct sockaddr_in clientAddr, serverAddr;

	int    opt, len, received, sockfd;

	char   recvBuffer[BufferSize];
	char   sendBuffer[BufferSize];

	char  *configfd = "server.cfg";
	MYSQL *dbCon = mysql_init(NULL);

	char  *nlpos;
	time_t ltime;
	char  *timestamp;


	puts(" _______ _______ _______ _______         _______ ______");
	puts("|     __|    |  |    ___|     __|.-----.|_     _|   __ \\");
	puts("|__     |       |    ___|__     ||  _  | _|   |_|    __/");
	puts("|_______|__|____|_______|_______||_____||_______|___|   server\n");
	puts("-c <config>\tload specific config file (defalt: server.cfg).\n");


	// Initialise database connection.
	if (! dbCon) {
		fprintf(stderr, "%s\n", mysql_error(dbCon));
		return EXIT_FAILURE;
	}


	// Parse command-line arguments.
	while ((opt = getopt(argc, argv, "c:")) != -1)
		switch (opt) {
			case 'c':
				configfd = optarg;
				break;
		}


	// Iinitialise configuration file.
	if (initConfig(configfd) < 0)
		switch (initConfig(configfd)) {
			case ErrorIO:
				printf("%s: wrong file format or file does not exist.\n", configfd);
				return EXIT_FAILURE;
			case ErrorMissingHostname:
				printf("%s: hostname is not set\n", configfd);

			case ErrorMissingUsername:
				printf("%s: username is not set\n", configfd);

			case ErrorMissingPassword:
				printf("%s: password is not set\n", configfd);

			case ErrorMissingDatabase:
				printf("%s: database is not set\n", configfd);

			default:
				return EXIT_FAILURE;
		}
	printf("Sucessfully laoded %s.\n", configfd);


	// Establish database connection.
	if (mysql_real_connect(dbCon, dbHostname, dbUsername, dbPassword,
			NULL, 0, NULL, 0) == NULL) {

		fprintf(stderr, "%s\n", mysql_error(dbCon));
		mysql_close(dbCon);
		return EXIT_FAILURE;
	}
	puts("Database connection established.");


	// Setting up the server.
	sockfd                     = socket(AF_INET, SOCK_DGRAM, 0);
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port        = htons(57350);

	if ((bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) == -1) {
		error("Couldn't bind name to socket: ");
		return EXIT_FAILURE;

	} else
		puts("Listening on port 57350.\n");


	while (1) {
		len = sizeof(clientAddr);
		received = recvfrom(sockfd, recvBuffer, BufferSize, 0, (struct sockaddr *)&clientAddr, &len);

		if (received == -1) {
			error("Couldn't receive message: ");
			continue;
		}



		//sendto(sockfd, &data, BufferSize, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));



		// Format timestamp.
		ltime     = time(NULL);
		timestamp = ctime(&ltime);
		nlpos     = strstr(timestamp, "\n");
		strncpy(nlpos, "\0", 1);
	}


	close(sockfd);
	mysql_close(dbCon);
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
