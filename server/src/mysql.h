/* mysql.h -*-c-*-
 * Database handler.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef MYSQL_h
#define MYSQL_h

#define QueryBufferSize 256


#include <libconfig.h>
#include <mysql.h>
#include <stdlib.h>
#include <syslog.h>
#include "utils.h"


MYSQL *dbCon;


void *keepaliveMySQL(void* val);

int   initMySQL(char *confFile);
void  finiMySQL();

int   setOnlineMySQLQuery(char *user, int status);
int   setPortMySQLQuery(char *user, int port);
int   setIPMySQLQuery(char *user, char *ip);

int   freePortMySQLQuery(char *curr_ip, int port);

int   userMySQLQuery(char *user);

int   setDateMySQLQuery(char *user);

char *getDestAddrMySQLQuery(char *dest_user);
char *getModeMySQLQuery(char *user, char *dest_user);
char *getOpponentMySQLQuery(char *user);

int   isPlayerOnlineMySQLQuery(char *user);
char *getClientKeyMySQLQuery(char *user);
int   authMySQLQuery(char *user, char *pass);

int   resetAllMySQLQuery(void);
int   resetAllIPMySQLQuery(void);
int   resetAllOnlineMySQLQuery(void);
int   resetAllPortMySQLQuery(void);


#endif
