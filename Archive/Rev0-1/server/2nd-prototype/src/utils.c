/* utils.c -*-c-*-
 * Various nonspecific helper functions.
 * Author: Michael Fitzmayer
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "utils.h"


int isValidIP(char *ipAddr) {
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddr, &(sa.sin_addr));


	return result != 0;
}


char *getTimestamp() {
	char*    nlpos;
	char*    timestamp;
	time_t   ltime;


	ltime     = time(NULL);
	timestamp = ctime(&ltime);
	nlpos     = strstr(timestamp, "\n");
	strncpy(nlpos, "\0", 1);


	return timestamp;
}
