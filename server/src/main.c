/* main.c -*-c-*-
 * SNESoIP server.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "main.h"


int main(int argc, char *argv[]){
	extern MYSQL *dbCon;
	pthread_t     t_sql_keepavlive;
	int           options, rc;
	char         *confFile = "server.conf";


	// Error logging.
	openlog(NULL, LOG_PERROR, LOG_WARNING);

  syslog(LOG_INFO, " _______ _______ _______ _______         _______ ______");
  syslog(LOG_INFO, "|     __|    |  |    ___|     __|.-----.|_     _|   __ \\");
  syslog(LOG_INFO, "|__     |       |    ___|__     ||  _  | _|   |_|    __/");
  syslog(LOG_INFO, "|_______|__|____|_______|_______||_____||_______|___|   server\n");
  syslog(LOG_INFO, "-c <config>\tload specific config file (defalt: server.conf).\n\n");

	// Parse command-line arguments.
	while ( (options = getopt(argc, argv, "c:") ) != -1)
		switch (options) {
			case 'c':
				confFile = optarg;
				break;
		}


	// Initialise MySQL.
	if ( initMySQL(confFile) == -1 )
		return -1;

	rc = pthread_create( &t_sql_keepavlive, NULL, &keepaliveMySQL, NULL );
	if ( rc != 0 ) {
		syslog(LOG_ERR, "Couldn't create t_sql_keepavlive.\n");
		return EXIT_FAILURE;
	}


	// Start TCP server.
	loop(init_tcp(PORT));


	// Never reached.
	closelog ();
	return 0;
}
