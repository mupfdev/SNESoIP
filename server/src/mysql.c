/* mysql.c -*-c-*-
 * Database handler.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "mysql.h"



int initMySQL(char *confFile) {
	const char *hostname;
	const char *username;
	const char *password;
	const char *database;

	char error = 0;
	dbCon = mysql_init(NULL);


	// Initialise and read configuration file.
	config_t          conf;
	config_setting_t *setting;
	config_init(&conf);

	syslog(LOG_INFO, "Step one, we can have lots of fun:");


	if (! config_read_file(&conf, confFile)) {
		config_destroy(&conf);
		syslog(LOG_ERR, "... %s: wrong file format or file does not exist.\n", confFile);
		return -1;
	}

	if ( (! config_lookup_string(&conf, "hostname", &hostname) ) || (strlen(hostname) == 0) ) {
		syslog(LOG_ERR, "... %s: hostname is not set\n", confFile);
		error = 1;
	}

	if ( (! config_lookup_string(&conf, "username", &username) ) || (strlen(username) == 0) ) {
		syslog(LOG_ERR, "... %s: username is not set\n", confFile);
		error = 1;
	}

	if ( (! config_lookup_string(&conf, "password", &password) ) || (strlen(password) == 0) ) {
		syslog(LOG_ERR, "... %s: password is not set\n", confFile);
		error = 1;
	}

	if ( (! config_lookup_string(&conf, "database", &database) ) || (strlen(database) == 0) ) {
		syslog(LOG_ERR, "... %s: database is not set\n", confFile);
		error = 1;
	}

	if (error == 1) return -1;
	syslog(LOG_INFO, "... sucessfully loaded %s.\n", confFile);


	// Initialise database connection.
	if (! dbCon) {
		syslog(LOG_ERR, "... %s\n", mysql_error(dbCon) );
		return -1;
	}
	syslog(LOG_INFO, "Step two, there's so much we can do:");

	// Establish database connection.
	if (mysql_real_connect(dbCon, hostname, username, password,
			database, 0, NULL, 0) == NULL) {

		syslog(LOG_ERR, "... %s\n", mysql_error(dbCon) );
		mysql_close(dbCon);
		return -1;
	}
	syslog(LOG_INFO, "... Database connection established.");

	syslog(LOG_INFO, "Step three, it's just you for me:");

	int ret = resetAllMySQLQuery();
	if(ret==0)
		syslog(LOG_INFO, "... reset status for all users.");
	else
		syslog(LOG_INFO, "... reset status for all users faild.");

	//syslog(LOG_INFO, "Step five, don't you know that the time has arrived");

	config_destroy(&conf);
	return 0;
}



void finiMySQL() { mysql_close(dbCon); }




int authMySQLQuery(char *user, char *pass) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	char *pass_esc = (char*) malloc(strlen(pass));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));
	mysql_real_escape_string(dbCon, pass_esc, pass, strlen(pass));

	int cx = snprintf ( query, 128, "SELECT userid FROM snesoip.user WHERE username = '%s' AND password = '%s'", user_esc , pass_esc );


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return 0;
	}


	mysql_free_result(res);

	if(atoi(row[0]) != 0);
	user_id = atoi(row[0]);

	return user_id;
}



int userMySQLQuery(char *user) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	int cx = snprintf ( query, 128, "SELECT userid FROM snesoip.user WHERE username = '%s'", user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return 0;
	}


	mysql_free_result(res);

	if(atoi(row[0]) != 0);
	user_id = atoi(row[0]);

	return user_id;
}



// Obsolete.
char *getModeMySQLQuery(char *user, char *dest_user) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));
	char *dest_user_esc = (char*) malloc(strlen(dest_user));
	mysql_real_escape_string(dbCon, dest_user_esc, dest_user, strlen(dest_user));

	//	int cx = snprintf ( query, 256,"SELECT CASE WHEN (SELECT auth_time from snesoip.user WHERE username='%s')>(SELECT auth_time from snesoip.user where username=(SELECT dest_username from snesoip.user where username='%s')) THEN 'true' ELSE 'false' END", user_esc, user_esc);
	int cx = snprintf ( query, 256,"SELECT CASE WHEN (SELECT auth_time from snesoip.user WHERE username='%s')>(SELECT auth_time from snesoip.user where username='%s') THEN 'true' ELSE 'false' END", user_esc, dest_user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}


	mysql_free_result(res);

	return row[0];
}



int isPlayerOnlineMySQLQuery(char *user) {
	char       query[QueryBufferSize];
	int        online = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	//	int cx = snprintf ( query, 256,"SELECT CASE WHEN (SELECT auth_time from snesoip.user WHERE username='%s')>(SELECT auth_time from snesoip.user where username=(SELECT dest_username from snesoip.user where username='%s')) THEN 'true' ELSE 'false' END", user_esc, user_esc);
	int cx = snprintf ( query, 256,"SELECT online from snesoip.user where username='%s'", user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return 0;
	}


	mysql_free_result(res);

	if(atoi(row[0]) != 0);
	online = atoi(row[0]);

	return online;
}



char *getDestAddrMySQLQuery(char *dest_user) {
	char       query[QueryBufferSize];

	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *dest_user_esc = (char*) malloc(strlen(dest_user));
	mysql_real_escape_string(dbCon, dest_user_esc, dest_user, strlen(dest_user));

	int cx = snprintf ( query, 256, "SELECT CONCAT(user.curr_ip,':',user.port) FROM user WHERE username='%s'", dest_user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}


	mysql_free_result(res);
	return row[0];
}



char *getClientKeyMySQLQuery(char *user) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	int cx = snprintf ( query, 256, "SELECT user.key FROM user WHERE username='%s'", user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}


	mysql_free_result(res);
	return row[0];
}



char *getOpponentMySQLQuery(char *user) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	int cx = snprintf ( query, 256, "SELECT user.dest_username FROM user WHERE username='%s'", user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return NULL;
	}


	mysql_free_result(res);
	return row[0];
}



int freePortMySQLQuery(char *curr_ip, int port) {
	char       query[QueryBufferSize];
	int        user_id = 0;
	MYSQL_RES *res;
	MYSQL_ROW  row;

	char *curr_ip_esc = (char*) malloc(strlen(curr_ip));
	mysql_real_escape_string(dbCon, curr_ip_esc, curr_ip, strlen(curr_ip));


	int cx = snprintf ( query, 128, "SELECT userid FROM snesoip.user WHERE curr_ip = '%s' AND port = %i", curr_ip_esc , port );


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "a%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (res = mysql_store_result(dbCon)) == NULL) {
		syslog(LOG_ERR, "b%s", mysql_error(dbCon) );
		return -1;
	}

	if ( (row = mysql_fetch_row(res)) == NULL) {
		//just not found
		//syslog(LOG_ERR, "c%s", mysql_error(dbCon) );
		return 0;
	}


	mysql_free_result(res);

	if(atoi(row[0]) != 0);
	user_id = atoi(row[0]);

	return user_id;
}



int resetAllMySQLQuery(void) {
	int a =  resetAllOnlineMySQLQuery();
	int b =  resetAllIPMySQLQuery();
	int c =  resetAllPortMySQLQuery();

	return (a+b+c);
}



int resetAllOnlineMySQLQuery(void) {
	char query[QueryBufferSize];

	int cx=0;
	cx = snprintf ( query, 128, "UPDATE snesoip.user SET online = false");

	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int resetAllIPMySQLQuery(void) {
	char query[QueryBufferSize];

	int cx=0;
	cx = snprintf ( query, 128, "UPDATE snesoip.user SET curr_ip = 'none'");

	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int resetAllPortMySQLQuery(void) {
	char query[QueryBufferSize];

	int cx=0;
	cx = snprintf ( query, 128, "UPDATE snesoip.user SET port = 0");

	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int setOnlineMySQLQuery(char *user, int status) {
	char query[QueryBufferSize];

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	if(status==1) {
		int cx = snprintf ( query, 128, "UPDATE snesoip.user SET online = true WHERE username = '%s'", user_esc);

	} else {
		int cx = snprintf ( query, 128, "UPDATE snesoip.user SET online = false WHERE username = '%s'", user_esc);
	}

	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int setPortMySQLQuery(char *user, int port) {
	char query[QueryBufferSize];

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	int cx = snprintf ( query, 128, "UPDATE snesoip.user SET port = %i WHERE username = '%s'", port, user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int setDateMySQLQuery(char *user) {
	char query[QueryBufferSize];

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));

	int cx = snprintf ( query, 128, "UPDATE snesoip.user SET auth_time = NOW() WHERE username = '%s'", user_esc);


	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



int setIPMySQLQuery(char *user, char *ip) {

	char query[QueryBufferSize];

	char *user_esc = (char*) malloc(strlen(user));
	mysql_real_escape_string(dbCon, user_esc, user, strlen(user));
	char *ip_esc = (char*) malloc(strlen(ip));
	mysql_real_escape_string(dbCon, ip_esc, ip, strlen(ip));

	int cx = snprintf ( query, 128, "UPDATE snesoip.user SET curr_ip = '%s' WHERE username = '%s'", ip_esc, user_esc);

	if (mysql_query(dbCon, query) != 0) {
		syslog(LOG_ERR, "%s", mysql_error(dbCon) );
		return -1;
	}


	return 0;
}



void *keepaliveMySQL(void* val) {
	int ret=0;
	syslog(LOG_INFO, "Step four, I can give you more");

	syslog(LOG_INFO, "...starting mysql keep alive\n");

	while(1){
		//syslog(LOG_INFO, "PING");
		ret = mysql_ping(dbCon);
		if(ret==0){
			//syslog(LOG_INFO, "PONG");
		} else
			if(ret==2006){
				syslog(LOG_ERR, "...sqlserver is gone!");

			} else
					syslog(LOG_ERR, "...keep alive - connection error!");

		sleep(60);
	}
	//  syslog(LOG_ERR, "...starting keep alive thread has faild\n");


	return NULL;
}
