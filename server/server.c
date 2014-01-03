/* server.c -*-c-*-
 * SNESoIP server.
 * Author: Michael Fitzmayer
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "server.h"


int main(int argc, char* argv[]) {
	struct sockaddr_in clientAddr, serverAddr;

	int    opt, len, received, sockfd;

	char   recvBuffer[6];
	char   sendBuffer[20];

	char  *configfd = "server.cfg";
	MYSQL *dbCon = mysql_init(NULL);


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
				fprintf(stderr, "%s: wrong file format or file does not exist.\n", configfd);
				return EXIT_FAILURE;
			case ErrorMissingHostname:
				fprintf(stderr, "%s: hostname is not set\n", configfd);

			case ErrorMissingUsername:
				fprintf(stderr, "%s: username is not set\n", configfd);

			case ErrorMissingPassword:
				fprintf(stderr, "%s: password is not set\n", configfd);

			case ErrorMissingDatabase:
				fprintf(stderr, "%s: database is not set\n", configfd);

			default:
				return EXIT_FAILURE;
		}
	printf("Sucessfully laoded %s.\n", configfd);


	// Establish database connection.
	if (mysql_real_connect(dbCon, dbHostname, dbUsername, dbPassword,
			dbDatabase, 0, NULL, 0) == NULL) {

		fprintf(stderr, "%s\n", mysql_error(dbCon));
		mysql_close(dbCon);
		return EXIT_FAILURE;
	}
	puts("Database connection established.");




	// Test queries.






	// Setting up the server.
	sockfd                     = socket(AF_INET, SOCK_DGRAM, 0);
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port        = htons(57350);

	if ((bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) == -1) {
		fprintf(stderr, "Couldn't bind name to socket: %s\n", strerror(errno));
		return EXIT_FAILURE;

	} else
		puts("Listening on port 57350.\n");


	while (1) {
		len = sizeof(clientAddr);
		received = recvfrom(sockfd, recvBuffer, BufferSize, 0, (struct sockaddr *)&clientAddr, &len);

		if (received == -1) {
			fprintf(stderr, "Couldn't receive message: %s\n", strerror(errno));
			continue;
		}



		//Commands: HELLO[ID] : login

		//getip:   "SELECT currentip FROM snesoip_hw WHERE hwid = %hwid%";
		//setip:   "UPDATE snesoip_hw SET currentip = '%ip%' WHERE hwid = %hwid%;"
		//getuser: "


		//sendto(sockfd, &data, BufferSize, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));





	}


	close(sockfd);
	mysql_close(dbCon);
	return EXIT_SUCCESS;
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
