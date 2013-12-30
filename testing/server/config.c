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
		return ErrorHostnameNotDefined;

	if (strlen(hostname) == 0)
		return ErrorHostnameNotDefined;

  if (! config_lookup_string(&cfg, "username", &username))
		return ErrorUsernameNotDefined;

	if (strlen(username) == 0)
		return ErrorUsernameNotSet;

  if (! config_lookup_string(&cfg, "password", &password))
		return ErrorPasswordNotDefined;

	if (strlen(password) == 0)
		return ErrorPasswordNotSet;

  if (! config_lookup_string(&cfg, "database", &database))
		return ErrorDatabaseNotDefined;

	if (strlen(password) == 0)
		return ErrorDatabaseNotSet;


	return 0;
}
