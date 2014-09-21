/* tcp.c -*-c-*-
 * TCP handler.
 * Author: saturnu
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include "tcp.h"


int init_tcp(int port) {
  struct sockaddr_in sock;
  int listen_fd;
  int ret;

  int yes = 1;

  listen_fd = socket(PF_INET, SOCK_STREAM, 0);
  exit_if(listen_fd < 0);

  ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  exit_if(ret < 0);

  memset((char *) &sock, 0, sizeof(sock));
  sock.sin_family = AF_INET;
  sock.sin_addr.s_addr = htonl(INADDR_ANY);
  sock.sin_port = htons(port);

  ret = bind(listen_fd, (struct sockaddr *) &sock, sizeof(sock));
  exit_if(ret != 0);

  ret = listen(listen_fd, 5);
  exit_if(ret < 0);

  return listen_fd;
}


int tcp_fw(int listen_fd) {
  int fd;
  struct sockaddr_in sock;
  socklen_t socklen;

  socklen = sizeof(sock);
  fd = accept(listen_fd, (struct sockaddr *) &sock, &socklen);
  return_if(fd < 0, -1);

  return fd;
}


int write_tcp(int fd, char buf[], int buflen) {
  int ret;

  ret = write(fd, buf, buflen);
  return_if(ret != buflen, -1);
  return 0;
}


void *tcp_server_read(void *arg) {
  long rfd;
  char buf[MAXLEN];
  int  buflen;
  //int wfd; //only for broadcast

  char username[32];
  char curr_ip[16];
  char session_string[11];

  char user_set=0;
  char greet_set=0;
  char auth_set=0;
  int  port_set=0;

  int brutforce_counter=0;

  rfd = (long)arg;
  for(;;) {


    buflen = read(rfd, buf, sizeof(buf));


    if (buflen <= 0 || brutforce_counter==5) {
      pthread_mutex_lock(&m_state);
      FD_CLR(rfd, &the_state);   //remove dead client

      	if (DEBUG)
		 syslog(LOG_INFO, "t_server: client dead        [%d]\n", rfd);

      		if(user_set){
				int ret = setOnlineMySQLQuery(username, 0);
				int ipr = setIPMySQLQuery(username, "none");
				int rep = setPortMySQLQuery(username, 0);
			}

      pthread_mutex_unlock(&m_state);
      close(rfd);
      pthread_exit(NULL);
    }


		//USER
		if(buf[0]=='U' && buf[1]=='S' && buf[2]=='E' && buf[3]=='R' && greet_set && user_set==0){

			 int exist=0;
			 buf[buflen-1]='\0'; //cut off newline

				if(strlen(buf) >= 5 + USERNAME_MIN){

				 if(strlen(buf) <= 5 + USERNAME_MAX){

					 if(!user_set) {

						snprintf ( username, 32, "%s", buf+5);
						exist = userMySQLQuery( username );
						}

						if (DEBUG) {
						 syslog(LOG_INFO, " ");
						 syslog(LOG_INFO, "t_server: user               [%s]\n", username);
						}

						if(exist==-1){
								user_set=0;
								char err[10]; snprintf(err, 11, "ERROR %d\n", USERNAME_ERROR);
								write_tcp(rfd, err, 10);
						}else{
							if(exist!=0){

									int online;
									online = isPlayerOnlineMySQLQuery(username);

									if(online!=1){

										user_set=1;
										write_tcp(rfd, "OK\n", 3);


										srand(time(NULL));

										int i;
										for( i = 0; i < 10; ++i){
											char c = '0' + rand()%72;
											if( c == '\\') //prevent escaping
											c='c';
											session_string[i] = c;
										}

									//	snprintf(session_string,10,"T<Si?TI36Z");
										session_string[10] = '\n';

									//	char session_string_[10];
									//	snprintf(session_string_,11,"%s\n",session_string);


										write_tcp(rfd, session_string, 11); //32c + newline
									//	write_tcp(rfd, session_string_, strlen(session_string_)); //32c + newline


									}
									else{
										user_set=0;
										char err[10]; snprintf(err, 11, "ERROR %d\n", USERNAME_ONLINE);
										write_tcp(rfd, err, 10);
									}
							}else{
								user_set=0;
								char err[10]; snprintf(err, 11, "ERROR %d\n", USERNAME_WRONG);
								write_tcp(rfd, err, 10);

							}
						}

				}else{
					user_set=0;
					char err[10]; snprintf(err, 11, "ERROR %d\n", USERNAME_TOO_LONG); //>32c
					printf(err);
					write_tcp(rfd, err, 10);
				}
			}
			else{
				user_set=0;
				char err[10]; snprintf(err, 11, "ERROR %d\n", USERNAME_TOO_SHORT);
				write_tcp(rfd, err, 10);
			}
		}


		//PASS
		if(buf[0]=='P' && buf[1]=='A' && buf[2]=='S' && buf[3]=='S' && greet_set && user_set && auth_set==0){

			char passwd_hex[128];
			buf[buflen-1]='\0';

				if(strlen(buf) >= 5 + PASSWORD_MIN * 2){ // min 10chr passwd


					 if(strlen(buf) <= 5 + PASSWORD_MAX * 2){ //max pw len

							//crypt passwd from client
							snprintf ( passwd_hex, 128, "%s", buf+5); //USER XXXX [5]
							int real_length=strlen(passwd_hex)/2;

							char *passwd = (char*) malloc(strlen(passwd_hex)*2);


							int h=0;
							for(h=0;h<strlen(passwd_hex);h+=2){
								char buf[5] = {'0', 'x', passwd_hex[h], passwd_hex[h+1], 0};
								passwd[h/2] = strtol(buf, NULL, 0);
							}


							//key from db
							char key[11];
							snprintf(key, 11, getClientKeyMySQLQuery(username));

							if (DEBUG) {
							 syslog(LOG_INFO, "t_server: trivium_init_iv    [%s]\n", session_string);
							 syslog(LOG_INFO, "t_server: trivium_l_init_iv  [%d]\n", strlen(session_string));
							 syslog(LOG_INFO, "t_server: trivium_key        [%s]\n", key);
							 syslog(LOG_INFO, "t_server: trivium_l_key      [%d]\n", strlen(key));
							 syslog(LOG_INFO, "t_server: password_crypt_hex [%s]\n", passwd_hex);
							 syslog(LOG_INFO, "t_server: password_length    [%d]\n", strlen(passwd_hex)/2);
							}

							trivium_ctx_t ctx;

							trivium_init(key, 80, session_string, 80, &ctx);
							trivium_enc(&ctx);

							int g=0;
							char passwd_[real_length];

							for(g=0; g<real_length;g++)
							passwd_[g] = passwd[g] ^ trivium_getbyte(&ctx);

							if (DEBUG)
							syslog(LOG_INFO, "t_server: password_plain     [%s]\n", passwd_);

							int auth = authMySQLQuery(username, passwd_);

							if(auth==-1){
										char err[10]; snprintf(err, 11, "ERROR %d\n", PASSWORD_ERROR);
										write_tcp(rfd, err, 10);
							}
							else{
								if(auth!=0){

									if (DEBUG)
									syslog(LOG_INFO, "t_server: access             [ok]\n");

									//login successful
									auth_set=1;

									//set_ip
									setIPMySQLQuery(username, curr_ip);

									write_tcp(rfd, "OK\n", 3);
									} else {
										if (DEBUG)
										syslog(LOG_INFO, "t_server: access             [failed]\n");
										user_set=0;
										brutforce_counter++;

										char err[10]; snprintf(err, 11, "ERROR %d\n", PASSWORD_WRONG);
										write_tcp(rfd, err, 10);
									}
							}

					}//length in rage
					else{//too long

						//user_set=0;
						char err[10]; snprintf(err, 11, "ERROR %d\n", PASSWORD_TOO_LONG);
						write_tcp(rfd, err, 10);

					}//max pw len

				}// min 10 length pw
				else{
					//user_set=0;
					char err[10]; snprintf(err, 11, "ERROR %d\n", PASSWORD_TOO_SHORT);
					write_tcp(rfd, err, 10);
				}

		}

		if(buf[0]=='P' && buf[1]=='O' && buf[2]=='R' && buf[3]=='T' && greet_set  && user_set  && auth_set && port_set==0){


			if (DEBUG)
			syslog(LOG_INFO, "t_server: port cmd           [rec]\n");

			int port_set=0;

			buf[buflen]='\0';

			if(strlen(buf) >= 5 + PORT_STR_MIN){

			char tmp_buf[MAXLEN];
			int tmp_buflen=0;

			int port=0;

				if(strlen(buf) <= 5 + PORT_STR_MAX)

				if (DEBUG)
				syslog(LOG_INFO, "t_server: port	           [%s]\n",buf);
				port = atoi(buf+5);

					if(port<=65535 && port>=10){ //max port 16bit

						int uid = freePortMySQLQuery(curr_ip, port);

							if(uid==0){
								int ret = setPortMySQLQuery(username, port);
								port_set=1; //port set

								if (DEBUG)
								syslog(LOG_INFO, "t_server: port	           [%d]\n",port);

								int reo = setOnlineMySQLQuery(username, 1);
								int red = setDateMySQLQuery(username);

								char dest_user[32];
								snprintf(dest_user, 32, getOpponentMySQLQuery(username));


								char dest_user_reverse[32];

								//if(DEBUG)
								//syslog(LOG_WARNING, "dbg: dest_player:%s",dest_user);
								int hosted_game=0;

								snprintf(dest_user_reverse, 32, getOpponentMySQLQuery(dest_user));

								if(strcmp("gameserver", dest_user_reverse) == 0)
								hosted_game=1;

								//if(DEBUG)
								//syslog(LOG_WARNING, "dbg: dest_user_reverse:%s",dest_user_reverse);


								if(strcmp(username, dest_user_reverse) != 0 && hosted_game==0) {

								char err[10]; snprintf(err, 11, "ERROR %d\n", OPPONENT_MISMATCH);

								if (DEBUG)
								syslog(LOG_INFO, "t_server: opponent match     [failed]\n");


								write_tcp(rfd, err, 10);

									// Cleanup.
									if(user_set){
										int ret = setOnlineMySQLQuery(username, 0);
										int ipr = setIPMySQLQuery(username, "none");
										int rep = setPortMySQLQuery(username, 0);
									}

								  pthread_mutex_lock(&m_state);
								  FD_CLR(rfd, &the_state);
								  pthread_mutex_unlock(&m_state);
								  close(rfd);
								  pthread_exit(NULL);

								}else{


									int online;
									online = isPlayerOnlineMySQLQuery(dest_user);

									//if (DEBUG)
									//syslog(LOG_WARNING, "dbg: isOnline:%d",online);

									if(online==-1){
											port_set=0;
											char err[10]; snprintf(err, 11, "ERROR %d\n", OPPONENT_ERROR);
											write_tcp(rfd, err, 10);
									}else{

										if(online==1){
										write_tcp(rfd, "SLAVE\n", 6);
										char dest_addr[32];
										snprintf(dest_addr, 32, getDestAddrMySQLQuery(dest_user));

										if (DEBUG)
										syslog(LOG_INFO, "t_server: mode               [slave]\n");

										char tcpip_addr[32];
										snprintf(tcpip_addr, 32, "%s\n",dest_addr);

										if (DEBUG)
										syslog(LOG_INFO, "t_server: addr               [%s]\n",dest_addr);

										write_tcp(rfd, tcpip_addr, strlen(tcpip_addr));
										}
										else{

											if(hosted_game==0){

												if (DEBUG)
												syslog(LOG_INFO, "t_server: mode               [master]\n");
												write_tcp(rfd, "MASTER\n", 7);
											}
											else{

												port_set=0;
												char err[10]; snprintf(err, 11, "ERROR %d\n", OPPONENT_ERROR);
												write_tcp(rfd, err, 10);
											}


										}
									}
								}
							}
							else{ // Used.

								port_set=0;
								char err[10]; snprintf(err, 11, "ERROR %d\n", PORT_USED);
								write_tcp(rfd, err, 10);


							}
					}else{ // Too high.

						char err[10]; snprintf(err, 11, "ERROR %d\n", PORT_TOO_HIGH);
						write_tcp(rfd, err, 10);
					}
				}
				else{
						char err[10]; snprintf(err, 11, "ERROR %d\n", PORT_TOO_LOW);
						write_tcp(rfd, err, 10);
				}
		}


		//HELO
		if(buf[0]=='H' && buf[1]=='E' && buf[2]=='L' && buf[3]=='O' && greet_set==0){

			struct sockaddr_in sa;
			//struct sockaddr_in * restrict sa;
			int sa_len;

			sa_len = sizeof(sa);

			if (getpeername(rfd, (struct sockaddr *)&sa, &sa_len) == -1) {
			syslog(LOG_ERR, "getsockname() failed");
			}

			//inet_ntoa() isn't threadsafe
			 int block4 =  sa.sin_addr.s_addr&0xFF;
			 int block3 = (sa.sin_addr.s_addr&0xFF00)>>8;
			 int block2 = (sa.sin_addr.s_addr&0xFF0000)>>16;
			 int block1 = (sa.sin_addr.s_addr&0xFF000000)>>24;

			// IP -> Database.
			  char str [64];
			  int cx;

			  cx = snprintf ( str, 64, "HELO client:%i.%i.%i.%i\n", block4,block3,block2,block1 );
			  cx = snprintf ( curr_ip, 32, "%i.%i.%i.%i\0", block4,block3,block2,block1 );
			  greet_set=1;


			write_tcp(rfd, str, strlen(str));
		}

		// Quit: End of TCP Connection.
		if(buf[0]=='Q' && buf[1]=='U' && buf[2]=='I' && buf[3]=='T' ){

			if(user_set){
				int ret = setOnlineMySQLQuery(username, 0);
				int ipr = setIPMySQLQuery(username, "none");
				int rep = setPortMySQLQuery(username, 0);
			}

		  pthread_mutex_lock(&m_state);
		  FD_CLR(rfd, &the_state);
		  pthread_mutex_unlock(&m_state);
		  close(rfd);
		  pthread_exit(NULL);
		}

		// Broadcast.
		/*
		if(buf[0]=='B'){
			pthread_mutex_lock(&m_state);
			for (wfd = 3; wfd < MAXFD; ++wfd) {
			  if (FD_ISSET(wfd, &the_state) && (rfd != wfd)) {
				write_tcp(wfd, buf, buflen);
			  }
			}
			pthread_mutex_unlock(&m_state);
		}
		* */

  }


  return NULL;
}


void loop(int listen_fd) {
  pthread_t threads[MAXFD];

  FD_ZERO(&the_state);
  for (;;) {
    long rfd;
    void *arg;

    rfd = tcp_fw(listen_fd);
    if (rfd >= 0) {
      if (rfd >= MAXFD) {
        close(rfd);
        continue;
      }
      pthread_mutex_lock(&m_state);
      FD_SET(rfd, &the_state);
      pthread_mutex_unlock(&m_state);

      arg = (void *) rfd;

      pthread_create(&threads[rfd], NULL, tcp_server_read, arg);
    }
  }
}
