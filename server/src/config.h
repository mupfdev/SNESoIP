/* config.h -*-c-*-
 * Configuration.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef CONFIG_h
#define CONFIG_h


#include <libconfig.h>
#include "mysql.h"


#define exit_if(expr) \
if (expr) { \
  syslog(LOG_WARNING, "exit_if() %s: %d: %s: Error %s\n", \
  __FILE__, __LINE__, __PRETTY_FUNCTION__, strerror(errno)); \
  exit(1); \
}


#define return_if(expr, retvalue)								\
if (expr) { \
  syslog(LOG_WARNING, "return_if() %s: %d: %s: Error %s\n\n", \
  __FILE__, __LINE__, __PRETTY_FUNCTION__, strerror(errno)); \
  return (retvalue); \
}


// Auth server port.
#define PORT 51234


// Buffer maxlength.
#define MAXLEN       1024

#define PASSWORD_MIN   10
#define PASSWORD_MAX   30

#define USERNAME_MIN    3
#define USERNAME_MAX   20

#define PORT_STR_MIN    2
#define PORT_STR_MAX    5


// Max threads.
#define MAXFD 128


// Debug.
#define DEBUG 1 // 1 = ON, 2 = OFF.


// Errors.
#define ERROR_BASE        100

#define USERNAME_TOO_LONG  ERROR_BASE + 11
#define USERNAME_TOO_SHORT ERROR_BASE + 12
#define USERNAME_WRONG     ERROR_BASE + 13
#define USERNAME_ERROR     ERROR_BASE + 14
#define USERNAME_ONLINE    ERROR_BASE + 15

#define PASSWORD_TOO_LONG  ERROR_BASE + 21
#define PASSWORD_TOO_SHORT ERROR_BASE + 22
#define PASSWORD_WRONG     ERROR_BASE + 23
#define PASSWORD_ERROR     ERROR_BASE + 24

#define PORT_TOO_HIGH      ERROR_BASE + 31
#define PORT_TOO_LOW       ERROR_BASE + 32
#define PORT_USED          ERROR_BASE + 33

#define OPPONENT_MISMATCH  ERROR_BASE + 41
#define OPPONENT_ERROR     ERROR_BASE + 42


#endif
