#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <sys/types.h>


#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __SVR4
	#include <sys/stropts.h>
#endif


#include <pthread.h>
#include <sched.h>
#include <semaphore.h>

#include "trivium.h"
#include "vsnesoip.h"

#define RCVBUFSIZE 128   /* Size of receive buffer */
//#define MAX_MSG_SIZE 257 // We will never send more than 256 bytes (+\0)

#define G(i) ((((*ctx)[(i)/8])>>(((i)%8)))&1)
#define S(i,v) ((*ctx)[(i)/8] = (((*ctx)[(i)/8]) & (uint8_t)~(1<<((i)%8))) | ((v)<<((i)%8)))
int sock;


//i am a thread after slave mode login
void *recv_opponent(void* val) {

	while(1){

		//incomming controller data
			if(!_SNESoIP_master){ // i'm slave
				socklen_t len;
				unsigned char remote_mesg[8];
			
				len = sizeof(_SNESoIP_client_addr);
				recvfrom(_SNESoIP_socket_tcp, remote_mesg,32,0,(struct sockaddr *)&_SNESoIP_server_addr,&len);
				
				unsigned char b0[2];
				unsigned char b1[2];
				b0[0] = remote_mesg[0];
				b0[1] = remote_mesg[1];
				b1[0] = remote_mesg[2];
				b1[1] = remote_mesg[3];
				
				unsigned char byte0 = strtol(b0, NULL, 16);
				unsigned char byte1 = strtol(b1, NULL, 16);
				
				process_incoming(byte0, byte1);
				
				//uint16_t bt = byte0 + (byte1 << 8) ;
				//printf("bt hack: [%d]\n",bt);
			}
	}

	return NULL;
}



void stop_snesoip()
{
  close(sock);
}

int init_login(char *confFile)
{
	
	const char *loginserver;
	const char *username;
	const char *password;
	const char *key;
	debug=0;
	/*
	 int *server_port;
	 int *tcp_min_port;
	 int *tcp_max_port;
*/


	// Initialise and read configuration file.
	config_t          conf;
	//config_setting_t *setting;
	config_init(&conf);


		if (! config_read_file(&conf, confFile)) {
		config_destroy(&conf);
		printf("... %s: wrong file format or file does not exist.\n", confFile);
		return -1;
	}
		if ( (! config_lookup_string(&conf, "loginserver", &loginserver) ) || (strlen(loginserver) == 0) ) {
		printf("... %s: loginserver is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_string(&conf, "username", &username) ) || (strlen(username) == 0) ) {
		printf("... %s: username is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_string(&conf, "password", &password) ) || (strlen(password) == 0) ) {
		printf("... %s: password is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_string(&conf, "key", &key) ) || (strlen(key) == 0) ) {
		printf("... %s: key is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_string(&conf, "js_dev", &js_dev) ) || (strlen(js_dev) == 0) ) {
		printf("... %s: js_dev is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_int(&conf, "server_port", &_SNESoIP_server_port) ) || _SNESoIP_server_port == 0)  {
		printf("... %s: server_port is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_int(&conf, "tcp_min_port", &_SNESoIP_tcp_min) ) || _SNESoIP_tcp_min == 0)  {
		printf("... %s: tcp_min_port is not set\n", confFile);
		return -1;
	}
		if ( (! config_lookup_int(&conf, "tcp_max_port", &_SNESoIP_tcp_max) ) || _SNESoIP_tcp_max == 0)  {
		printf("... %s: tcp_max_port is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_type", &js_axis_type))   {
		printf("... %s: js_axis_type is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_type", &js_button_type))   {
		printf("... %s: js_button_type is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_up_val", &js_axis_up_val))   {
		printf("... %s: js_axis_up_val is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_down_val", &js_axis_down_val))   {
		printf("... %s: js_axis_down_val is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_left_val", &js_axis_left_val))   {
		printf("... %s: js_axis_left_val is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_right_val", &js_axis_right_val))   {
		printf("... %s: js_axis_right_val is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_y_nr", &js_axis_y_nr))   {
		printf("... %s: js_axis_y_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_axis_x_nr", &js_axis_x_nr))   {
		printf("... %s: js_axis_x_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_a_nr", &js_button_a_nr))   {
		printf("... %s: js_button_a_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_b_nr", &js_button_b_nr))   {
		printf("... %s: js_button_b_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_x_nr", &js_button_x_nr))   {
		printf("... %s: js_button_x_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_y_nr", &js_button_y_nr))   {
		printf("... %s: js_button_y_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_l_nr", &js_button_l_nr))   {
		printf("... %s: js_button_l_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_r_nr", &js_button_r_nr))   {
		printf("... %s: js_button_r_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_st_nr", &js_button_st_nr))   {
		printf("... %s: js_button_st_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "js_button_se_nr", &js_button_se_nr))   {
		printf("... %s: js_button_se_nr is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "disable_p1", &disable_p1))   {
		printf("... %s: disable_p1 is not set\n", confFile);
		return -1;
	}
		if ( ! config_lookup_int(&conf, "select_x", &select_x))   {
		printf("... %s: select_x is not set\n", confFile);
		return -1;
	}	
		if ( ! config_lookup_int(&conf, "mapping_mode", &mapping_mode))   {
		printf("... %s: mapping_mode is not set\n", confFile);
		return -1;
	}		
		if  (! config_lookup_int(&conf, "debug", &debug) )  {
		printf("... %s: debug is not set\n", confFile);
		printf("... default value set\n");
	}



		_SNESoIP_master=0;	
		snprintf(_SNESoIP_server, 128, loginserver); 
		snprintf(_SNESoIP_username, 128, username); 
		snprintf(_SNESoIP_password, 128, password); 
		snprintf(_SNESoIP_key, 128, key); 


    struct sockaddr_in snesoIPServAddr; /* sockaddr_in for snesoip server address */
    unsigned int messageStringLen;      /* Length of string to echo */
    int totalBytesRcvd, bytesRcvd;      // (Total) Amount of data received from the server
    char messageBuffer[RCVBUFSIZE];     /* Buffer for message string */
    char sendMessage[MAX_MSG_SIZE];     // Buffer for outgoing messages
    pthread_t     t_opponent_inc;
    
    
    printf("%s\n","Initialize _SNESoIP...");
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      printf("Could notcreate TCP/IP Socket!");
      return FALSE;
    }
    
    memset(&snesoIPServAddr,0,sizeof(struct sockaddr_in));
    snesoIPServAddr.sin_family      = AF_INET;             /* Internet address family */
    snesoIPServAddr.sin_addr.s_addr = inet_addr(_SNESoIP_server);   /* Server IP address */
    snesoIPServAddr.sin_port        = htons(_SNESoIP_server_port); /* Server port */

	if(debug){
	fprintf(stdout, "-> _SNESoIP-key [%s]\n", _SNESoIP_key);
	fprintf(stdout, "-> _SNESoIP-username [%s]\n", _SNESoIP_username);
	fprintf(stdout, "-> _SNESoIP-password [%s]\n", _SNESoIP_password);
	fprintf(stdout, "-> _SNESoIP-server [%s]\n", _SNESoIP_server);		
	fprintf(stdout, "-> _SNESoIP-server_port [%d]\n", _SNESoIP_server_port);	
	fprintf(stdout, "-> _SNESoIP-server_port_min [%d]\n", _SNESoIP_tcp_min);	
	fprintf(stdout, "-> _SNESoIP-server_port_max [%d]\n", _SNESoIP_tcp_max);
	}

    if (connect(sock, (struct sockaddr *) &snesoIPServAddr, sizeof(snesoIPServAddr)) < 0)
    {
        printf("Connectection to _SNESoIP server failed\n");
	return FALSE;
    }

    printf("Connection to _SNESoIP server established\n");

    snprintf(sendMessage,MAX_MSG_SIZE,"HELO VSNESOIP\n");
    messageStringLen = strlen(sendMessage)+1;          /* Determine input length */
    
    /* Send the string to the server */
    if (send(sock, sendMessage, messageStringLen, 0) != messageStringLen)
    {
      printf("Server did not respond to helo ;_;\n");
      return FALSE;
    }

    totalBytesRcvd = 0;
    do 
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE - 1, 0)) <= 0)
        {
            printf("recv() failed or connection closed prematurely\n");
	    return FALSE;
        }

        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        messageBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    }while (messageBuffer[totalBytesRcvd-1] != '\n');

    //stage two USER LOGIN
    snprintf(sendMessage,MAX_MSG_SIZE,"USER %s",_SNESoIP_username);

    messageStringLen = strlen(sendMessage)+1;          /* Determine input length */
    printf("Try to login as %s\n",sendMessage);

    /* Send the string to the server */


    if (send(sock, sendMessage, messageStringLen, 0) != messageStringLen)
    {
        printf("Could not send user request to the server...\n");
	return FALSE;
    }

    totalBytesRcvd = 0;
    do 
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE - 1, 0)) <= 0)
        {
          printf("recv() failed or connection closed prematurely");
	  return FALSE;
        }

        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        messageBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    }while (messageBuffer[totalBytesRcvd-1] != '\n');
    
    if (strcmp("OK\n",messageBuffer) != 0)
    {
      printf("User request failed");
      return FALSE;
    }

    totalBytesRcvd = 0;
   do 
   {
        if ((bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE - 1, 0)) <= 0)
        {
           printf("recv() failed or connection closed prematurely");
	   return FALSE;
        }

        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        messageBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
    }while (totalBytesRcvd <= 10);
    
    
    // Now comes the trivium part

    //password
    char passwd[MAX_PASSWORD_LENGTH];
    
    //passwd="pass1";
    snprintf(passwd,MAX_PASSWORD_LENGTH,_SNESoIP_password);

    trivium_ctx_t ctx;

    trivium_init(_SNESoIP_key, 80, messageBuffer, 80, &ctx);

    trivium_enc(&ctx);

    int g=0;
    unsigned char passwd_[strlen(passwd)];
    for(g=0; g<strlen(passwd);g++)
    passwd_[g] = passwd[g] ^ trivium_getbyte(&ctx);

    //strlen of passwd_ might be wrong 'cause of string termination
    //dump(passwd_,strlen(passwd));


    char *raw_crypt = (char*) malloc(strlen(passwd)*2);
    memset(raw_crypt,0,strlen(passwd)*2);
    uint8_t *m = passwd_;
    int c = sizeof(passwd_);
    while(c--)
    {
        char chr[2];

        snprintf(chr, 3,"%02x", *(m++));
        strcat(raw_crypt, chr);
    }


    //just a reverse debug testrun
    //////////////
/*
    trivium_ctx_t ctxb;

    trivium_init(_SNESoIP_key, 80, messageBuffer, 80, &ctxb);
    trivium_enc(&ctxb);

    g=0;
    char passwd_p[strlen(passwd)];
    for(g=0; g<strlen(passwd);g++)
    passwd_p[g] = passwd_[g] ^ trivium_getbyte(&ctxb);
*/
    //////////////
    /////////////


    snprintf(sendMessage,MAX_MSG_SIZE,"PASS %s",raw_crypt);
    messageStringLen = strlen(sendMessage)+1;

    if (send(sock, sendMessage, messageStringLen, 0) != messageStringLen)
    {
        printf("send() sent a different number of bytes than expected\n");
	return FALSE;
    }

    totalBytesRcvd = 0;
    do
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE - 1, 0)) <= 0)
        {
           printf("recv() failed or connection closed prematurely\n");
           return FALSE;
        }

        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        messageBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        break;
    }while (messageBuffer[totalBytesRcvd-1] != '\n');

    // End of trivium part

    if(strcmp("OK\n",messageBuffer) == 0)
    {
        printf("Logged in succesfully\n");
    }
    else
    {
        printf("Username or password wrong!\n");
	return FALSE;
    }

    // Now we should get an TCP port
    int tcpPortFound = FALSE;
    int tcpPort = _SNESoIP_tcp_min;
    
    while (tcpPort <= _SNESoIP_tcp_max && !tcpPortFound)
    {
          snprintf(sendMessage,MAX_MSG_SIZE,"PORT %i\n",tcpPort);
	  printf("Try with %s",sendMessage);
	  messageStringLen = strlen(sendMessage)+1;
	  if (send(sock, sendMessage, messageStringLen, 0) != messageStringLen)
	  {
	    printf("send() sent a different number of bytes than expected\n");
	    return FALSE;
	  }
	    
	  totalBytesRcvd = 0;
	  do
	  {
	    if ((bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE - 1, 0)) <= 0)
	    {
	      
	      printf("recv() failed or connection closed prematurely\n");
	      return FALSE;
	    }
	    
	    totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
	    messageBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
	  }while(messageBuffer[bytesRcvd-1] != '\n');

	  if (strcmp("MASTER\n",messageBuffer) == 0 || strcmp("SLAVE\n",messageBuffer) == 0)
	  {
		  int size_i=0;
		   char *buffer = (char*)malloc (1024);
		   
		    if(strcmp("MASTER\n",messageBuffer) == 0){
						//  mode=1;
						
						  _SNESoIP_master = TRUE;
						  //no master mode now -__-
						  
						  }
						  else if(strcmp("SLAVE\n",messageBuffer) == 0){
						 // mode=2;
						  
						  _SNESoIP_master = FALSE;
						  
						  		totalBytesRcvd = 0;
								do //get port+ip
								{
									if ((size_i = recv(sock, buffer, MAX_MSG_SIZE-1, 0)) <= 0)
									{
									  
									  printf("recv() failed or connection closed prematurely\n");
										 return -1;
									}
									
									totalBytesRcvd += size_i;   /* Keep tally of total bytes */
									buffer[size_i] = '\0';  /* Terminate the string! */
								
								if(debug)
								  printf("debug: %s", buffer);	
									
								}while(buffer[size_i-1] != '\n');

								char ** res  = NULL;
								char *  p    = strtok (buffer, ":");
								int n_del = 0;


								/* split string and append tokens to 'res' */

								while (p) {
								  res = (char**) realloc (res, sizeof (char*) * ++n_del);

								  if (res == NULL)
									exit (-1); /* memory allocation failed */

								  res[n_del-1] = p;

								  p = strtok (NULL, " ");
								}

								/* realloc one extra element for the last NULL */

								res = (char**) realloc (res, sizeof (char*) * (n_del+1));
								res[n_del] = 0;

								/* free the memory allocated */
								
								snprintf(_SNESoIP_master_addr, 16, res[0]);
								
								_SNESoIP_master_port = atoi(res[1]);
								free (res);
								
						  }
		  
	    tcpPortFound = TRUE;
	  }
	  else
	  {
	    tcpPort++;
	    printf("That port was already in use...\n");
	  }
    }
    
    if (tcpPortFound)
    {
      printf("Use TCP-Port %i\n",tcpPort);
    }
    else
    {
      printf("Could not find any free TCP port to use...\n");
      return FALSE;

    }
		//I'm SLAVE
	  if(!_SNESoIP_master){
		  
		  //hack for socat monitoring
			//_SNESoIP_master_port=_SNESoIP_master_port+1;
		  
			if(debug){
				fprintf(stdout, "-> _SNESoIP-master_addr [%s]\n", _SNESoIP_master_addr);
				fprintf(stdout, "-> _SNESoIP-master_port [%d]\n", _SNESoIP_master_port);
			}

			_SNESoIP_socket_tcp=socket(AF_INET,SOCK_STREAM,0); //tcp client

		    bzero(&_SNESoIP_server_addr,sizeof( _SNESoIP_server_addr));
		    _SNESoIP_server_addr.sin_family = AF_INET;
		    _SNESoIP_server_addr.sin_addr.s_addr=inet_addr(_SNESoIP_master_addr);
		    _SNESoIP_server_addr.sin_port=htons(_SNESoIP_master_port);
		    
		    //connect to server
			connect(_SNESoIP_socket_tcp, (struct sockaddr *)&_SNESoIP_server_addr, sizeof(_SNESoIP_server_addr));

			//debug send
			//sendto(_SNESoIP_socket_tcp,"33\n",strlen("33\n"),0, (struct sockaddr *)&_SNESoIP_server_addr,sizeof(_SNESoIP_server_addr));
	

		int rc;
		rc = pthread_create( &t_opponent_inc, NULL, &recv_opponent, NULL );
		if ( rc != 0 ) {
			printf("Couldn't create t_opponent_inc.\n");
		}
		ready=1;

	
	
	  }else{ //master
		  
			//wait for client helo
			//if helo configure new partner

	  }
	  
	  
	  
	  
    close(sock);
    return 0;
}

uint8_t trivium_enc(trivium_ctx_t* ctx){
	uint8_t t1,t2,t3,z;

	t1 = G(65)  ^ G(92);
	t2 = G(161) ^ G(176);
	t3 = G(242) ^ G(287);
	z  = t1^t2^t3;
	t1 ^= (G(90)  & G(91))  ^ G(170);
	t2 ^= (G(174) & G(175)) ^ G(263);
	t3 ^= (G(285) & G(286)) ^ G(68);

	/* shift whole state and insert ts later */
	uint8_t i,c1=0,c2;
	for(i=0; i<36; ++i){
		c2=(((*ctx)[i])>>7);
		(*ctx)[i] = (((*ctx)[i])<<1)|c1;
		c1=c2;
	}
	/* insert ts */
	S(0, t3);
	S(93, t1);
	S(177, t2);

	return z?0x080:0x00;
}

uint8_t trivium_getbyte(trivium_ctx_t *ctx){
	uint8_t r=0, i=0;
	do{
		r>>=1;
		r |= trivium_enc(ctx);
	}while(++i<8);
	return r;
}

#define KEYSIZE_B ((keysize_b+7)/8)
#define IVSIZE_B  ((ivsize_b +7)/8)

//static const uint8_t rev_table[16] PROGMEM = {
	static const uint8_t rev_table[16]  = {
	0x00, 0x08, 0x04, 0x0C,   /* 0000 1000 0100 1100 */
	0x02, 0x0A, 0x06, 0x0E,   /* 0010 1010 0110 1110 */
	0x01, 0x09, 0x05, 0x0D,   /* 0001 1001 0101 1101 */
	0x03, 0x0B, 0x07, 0x0F    /* 0011 1011 0111 1111 */
};

void trivium_init(const void* key, uint16_t keysize_b,
                  const void* iv,  uint16_t ivsize_b,
                  trivium_ctx_t* ctx){
	uint16_t i;
	uint8_t c1,c2;
	uint8_t t1,t2;
	memset((*ctx)+KEYSIZE_B, 0, 35-KEYSIZE_B);
	c2=0;
	c1=KEYSIZE_B;
	do{
		t1 = ((uint8_t*)key)[--c1];
		//t2 = (pgm_read_byte(&(rev_table[t1&0x0f]))<<4)|(pgm_read_byte(&(rev_table[t1>>4])));
		t2 = (rev_table[t1&0x0f] << 4)|(rev_table[t1>>4]);
		(*ctx)[c2++] = t2;
	}while(c1!=0);

	c2=12;
	c1=IVSIZE_B;
	do{
		t1 = ((uint8_t*)iv)[--c1];
		//t2 = (pgm_read_byte(&(rev_table[t1&0x0f]))<<4)|(pgm_read_byte(&(rev_table[t1>>4])));
	t2 = (rev_table[t1&0x0f]<<4)|(rev_table[t1>>4]);
		(*ctx)[c2++] = t2;
	}while(c1!=0);

	for(i=12+IVSIZE_B; i>10; --i){
		c2=(((*ctx)[i])<<5);
		(*ctx)[i] = (((*ctx)[i])>>3)|c1;
		c1=c2;
	}

	(*ctx)[35] = 0xE0;

	for(i=0; i<4*288; ++i){
		trivium_enc(ctx);
	}
}


