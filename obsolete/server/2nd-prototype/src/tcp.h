/* tcp.h -*-c-*-
 * TCP handler.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef TCP_h
#define TCP_h



#include <errno.h>
#include <pthread.h>
#include "config.h"
#include "trivium.h"



volatile fd_set the_state;
pthread_mutex_t m_state = PTHREAD_MUTEX_INITIALIZER;



int   init_tcp(int port);
int   tcp_fw(int listen_fd);
int   write_tcp(int fd, char buf[], int buflen);
void *tcp_server_read(void *arg);
void  loop(int listen_fd);



#endif
