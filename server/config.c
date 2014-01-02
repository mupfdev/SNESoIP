/* config.c -*-c-*-
 * Configuration handler.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "config.h"


int initConfig(char *filename) {
	config_t cfg;
	config_setting_t *setting;


	config_init(&cfg);


	if (! config_read_file(&cfg, filename)) {
		config_destroy(&cfg);
		return ErrorIO;
	}


	if (! config_lookup_string(&cfg, "hostname", &dbHostname))
		return ErrorMissingHostname;

	if (strlen(dbHostname) == 0)
		return ErrorMissingHostname;

	if (! config_lookup_string(&cfg, "username", &dbUsername))
		return ErrorMissingUsername;

	if (strlen(dbUsername) == 0)
		return ErrorMissingUsername;

	if (! config_lookup_string(&cfg, "password", &dbPassword))
		return ErrorMissingPassword;

	if (strlen(dbPassword) == 0)
		return ErrorMissingPassword;

	if (! config_lookup_string(&cfg, "database", &dbDatabase))
		return ErrorMissingDatabase;

	if (strlen(dbDatabase) == 0)
		return ErrorMissingDatabase;


	return 0;
}
