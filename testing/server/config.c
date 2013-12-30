/* config.c -*-c-*-
 * Configuration handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#include "config.h"


int initConfig(char *filename) {
	config_t cfg;
	config_setting_t *setting;


	config_init(&cfg);


	if (! config_read_file(&cfg, filename)) {
		config_destroy(&cfg);
		return ErrorFileDoesNotExist;
	}


	if (! config_lookup_string(&cfg, "hostname", &hostname))
		return ErrorMissingHostname;

	if (strlen(hostname) == 0)
		return ErrorMissingHostname;

	if (! config_lookup_string(&cfg, "username", &username))
		return ErrorMissingUsername;

	if (strlen(username) == 0)
		return ErrorMissingUsername;

	if (! config_lookup_string(&cfg, "password", &password))
		return ErrorMissingPassword;

	if (strlen(password) == 0)
		return ErrorMissingPassword;

	if (! config_lookup_string(&cfg, "database", &database))
		return ErrorMissingDatabase;

	if (strlen(password) == 0)
		return ErrorMissingDatabase;


	return 0;
}
