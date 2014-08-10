/* utils.h -*-c-*-
 * Various nonspecific helper functions.
 * Author: Michael Fitzmayer
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef UTILS_h
#define UTILS_h



#include <arpa/inet.h>
#include <time.h>
#include <string.h>



int    isValidIP(char *ipAddr);
char  *getTimestamp();



#endif
