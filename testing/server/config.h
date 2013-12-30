/* config.h -*-c-*-
 * Configuration handler.
 * Copyright (c) 2013 Michael Fitzmayer.  All rights reserved.
 *
 * This program has has been released under the terms of a BSD-like
 * license.  See the file LICENSE for details. */


#ifndef CONFIG_h
#define CONFIG_h

#include <libconfig.h>
#include <string.h>


#define ErrorFileDoesNotExist   -1
#define ErrorMissingHostname    -2
#define ErrorMissingUsername    -3
#define ErrorMissingPassword    -4
#define ErrorMissingDatabase    -5


const char *hostname;
const char *username;
const char *password;
const char *database;


int initConfig(char *filename);


#endif
