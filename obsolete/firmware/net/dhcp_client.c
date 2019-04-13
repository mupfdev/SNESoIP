/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright:LGPL V2
 * See http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html
 *
 * A DHCP client.
 * This code uses the UDP_client framework. You need to enable UDP_client in ip_config.h to use this.
 *
 * DHCP requires the periodic renewal of addresses. For this 
 * purpose you must call the function dhcp_6sec_tick() every 6 sec.
 * If you do not call it then you can still get an initial IP
 * (without retry in case of failure) but there will be no
 * lease renewal.
 *********************************************/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include "net.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "ip_config.h"

#ifndef UDP_client
#error "ERROR: you need to enable UDP_client support in ip_config.h to use the DHCP client"
#endif

static volatile uint8_t dhcp_6sec_cnt=0; // counts multiple of 6 sec
static uint8_t dhcp_yiaddr[4]={0,0,0,0}; // your (client) IP
static uint8_t dhcp_opt_defaultgw[4]={0,0,0,0}; // default gw
static uint8_t dhcp_opt_mask[4]={0,0,0,0}; // netmask
static uint8_t dhcp_opt_server_id[4]={0,0,0,0}; // server ip
static uint8_t dhcp_opt_message_type=0;
static uint8_t dhcp_tid=0;
static uint16_t dhcp_opt_leasetime_minutes=0;

// 
// The relevant RFCs are 
// DHCP protocol: http://tools.ietf.org/html/rfc1541
// newer version:
// DHCP protocol: http://tools.ietf.org/html/rfc2131
// message encoding: http://tools.ietf.org/html/rfc1533
//
// The normal message flow to get an IP address is:
// Client -> Server DHCPDISCOVER
// Server -> Client DHCPOFFER
// Client -> Server DHCPREQUEST
// Server -> Client DHCPACK
// There might be serveral seconds delay between the DHCPDISCOVER and
// the DHCPOFFER as the server does some test (e.g ping or arp) to see
// if the IP which is offered is really free.
//
// At lease renewal the message flow is:
// Client -> Server DHCPREQUEST
// Server -> Client DHCPACK
// The DHCPREQUEST for renewal is different from the one at inital assignment.
//
//
// DHCP_OPTION_OFFSET is a relative to UDP_DATA_P
#define DHCP_OPTION_OFFSET 240
#define MAGIC_COOKIE_P 236
#define DHCP_SRV_SRC_PORT 67
#define DHCP_SRV_DST_PORT 68
// message type values
#define DHCP_DISCOVER_V 0x01
#define DHCP_REQUEST_V 0x03

// Lease time renewal and time keeping.
// you must call this function every 6 seconds. It is save
// to do this from interrupt
void dhcp_6sec_tick(void){
        dhcp_6sec_cnt++;
}

// This function writes a basic message template into buf
// It processes all fields excluding the options section.
// Most fields are initialized with zero.
void make_dhcp_message_template(uint8_t *buf,const uint8_t transactionID)
{
        uint8_t i=0;
        uint8_t allxff[6]={0xff,0xff,0xff,0xff,0xff,0xff}; // all of it can be used as mac, the first 4 can be used as IP

        send_udp_prepare(buf,DHCP_SRV_DST_PORT,allxff,DHCP_SRV_SRC_PORT,allxff);
        // source IP is 0.0.0.0, a while loop produces smaller code than memset
        i=0;
        while(i<4){
                buf[IP_SRC_P +i]=0;
                i++;
        }
        // now fill the bootstrap protocol layer starting at UDP_DATA_P
        buf[UDP_DATA_P]=1;// message type = boot request
        buf[UDP_DATA_P+1]=1;// hw type
        buf[UDP_DATA_P+2]=6;// mac len
        buf[UDP_DATA_P+i+3]=0;// hops
        // we use only one byte TIDs, we fill the first byte with 1 and
        // the rest with transactionID. The first byte is used to
        // distinguish inital requests from renew requests.
        buf[UDP_DATA_P+4]=1;
        i=0;
        while(i<3){
                buf[UDP_DATA_P+i+5]=transactionID;
                i++;
        }
        // we set seconds always to zero
        //
        // set my own MAC the rest is empty:
        // a while loop produces smaller code than memset
        i=8; // start at 8
        while(i<28){
                buf[UDP_DATA_P +i]=0;
                i++;
        }
        // own mac (send_udp_prepare did fill it at eth level):
        i=0;
        while(i<6){
                buf[UDP_DATA_P+i+28]=buf[ETH_SRC_MAC +i];
                i++;
        }
        // now we need to write 202 bytes of zero
        // a while loop produces smaller code than memset, don't use memset
        i=34; // start at 8
        while(i<236){
                buf[UDP_DATA_P +i]=0;
                i++;
        }
        // the magic cookie has to be:
        // 99, 130, 83, 99
        buf[UDP_DATA_P+MAGIC_COOKIE_P]=0x63;
        buf[UDP_DATA_P+MAGIC_COOKIE_P+1]=0x82;
        buf[UDP_DATA_P+MAGIC_COOKIE_P+2]=0x53;
        buf[UDP_DATA_P+MAGIC_COOKIE_P+3]=0x63;
}

// the answer to this message will come as a broadcast
uint8_t send_dhcp_discover(uint8_t *buf,const uint8_t transactionID)
{
        make_dhcp_message_template(buf,transactionID);
        // option dhcp message type:
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET]=0x35; // 53
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+1]=1; //len
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+2]=DHCP_DISCOVER_V;
        // option paramter request list:
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+3]=0x37; // 55
        // we want: subnet mask, router
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+4]=2; // len
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+5]=1; // subnet mask
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+6]=3; // router=default GW
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+7]=0xff; // end of options
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+8]=0; 
        // no padding
        // the length of the udp message part is now DHCP_OPTION_OFFSET+8
        send_udp_transmit(buf,DHCP_OPTION_OFFSET+8);
        return(0);
}

// scan the options field for the message type field
// and return its value.
//
// Value   Message Type
// -----   ------------
//   1     DHCPDISCOVER
//   2     DHCPOFFER  (server to client)
//   3     DHCPREQUEST
//   4     DHCPDECLINE
//   5     DHCPACK  (server to client)
//   6     DHCPNAK  (server to client)
//   7     DHCPRELEASE
// return 0 on message type not found otherwise the numeric
// value for the message type as shown in the table above.
uint8_t dhcp_get_message_type(uint8_t *buf,uint16_t plen)
{
        uint16_t option_idx;
        uint8_t option_len;
        // the smallest option is 3 bytes
        if (plen<(UDP_DATA_P+DHCP_OPTION_OFFSET+3)) return(0);
        // options are coded in the form: option_type,option_len,option_val
        option_idx=UDP_DATA_P+DHCP_OPTION_OFFSET;
        while(option_idx+2 <plen ){
                option_len=buf[option_idx+1];
                if ((option_len<1) || ((option_idx + option_len + 1)> plen)) break;
                if (buf[option_idx]==53){
                        // found message type, return it:
                        return(buf[option_idx+2]);
                }
                option_idx+=2+option_len;
        }
        return(0);
}

// use this on DHCPACK or DHCPOFFER messages to read "your ip address"
uint8_t dhcp_get_yiaddr(uint8_t *buf,uint16_t plen)
{
        // DHCP offer up to options section is at least 0x100 bytes
        if (plen<0x100) return(0);
        if (buf[UDP_DATA_P+16]!=0){
                // we have a yiaddr
                memcpy(dhcp_yiaddr, buf+UDP_DATA_P+16, 4);
                return(1);
        }
        return(0);
}

// this will as well update dhcp_yiaddr
uint8_t is_dhcp_msg_for_me(uint8_t *buf,uint16_t plen,const uint8_t transactionID)
{
        // DHCP offer up to options section is at least 0x100 bytes
        if (plen<0x100) return(0);
        if (buf[UDP_SRC_PORT_L_P] != DHCP_SRV_SRC_PORT) return(0);
        if (buf[UDP_DATA_P]!=2) return(0); // message type DHCP boot reply =2
        if (buf[UDP_DATA_P+5]!=transactionID) return(0);
        if (buf[UDP_DATA_P+6]!=transactionID) return(0);
        return(1);

}

// check if this message was part of a renew or 
uint8_t dhcp_is_renew_tid(uint8_t *buf,uint16_t plen)
{
        if (plen<0x100) return(0);
        if (buf[UDP_DATA_P+4]==2) return(1); // we did set first byte in transaction ID to 2 to indicate renew request. This trick makes the processing of the DHCPACK message easier.
        return(0);
}

uint8_t dhcp_option_parser(uint8_t *buf,uint16_t plen)
{
        uint16_t option_idx;
        uint8_t option_len;
        uint8_t i;
        uint32_t ltime_int32=0;
        // the smallest option is 3 bytes
        if (plen<(UDP_DATA_P+DHCP_OPTION_OFFSET+3)) return(0);
        // options are coded in the form: option_type,option_len,option_val
        option_idx=UDP_DATA_P+DHCP_OPTION_OFFSET;
        while(option_idx+2 <plen ){
                option_len=buf[option_idx+1];
                if ((option_len<1) || ((option_idx + option_len + 1)> plen)) break;
                switch (buf[option_idx]){
                case 0: option_idx=plen; // stop loop, we are reading some padding bytes here (should not happen)
                        break;
                     
                case 1: if (option_len==4){
                                i=0;
                                while(i<4){
                                        dhcp_opt_mask[i]=buf[option_idx+2+i];
                                        i++;
                                }
                        }
                        break;
                case 3: if (option_len==4){
                                i=0;
                                while(i<4){
                                        dhcp_opt_defaultgw[i]=buf[option_idx+2+i];
                                        i++;
                                }
                        }
                        break;
                // Lease time: throughout the protocol, times are to 
                // be represented in units of seconds.  The time value 
                // of 0xffffffff is reserved to represent "infinity". 
                // The max lease time size is therefore 32 bit. 
                // The code for this option is 51, and its length is 4
                // as per RFC 1533.
                case 51: 
                        if (option_len!=4){
                                dhcp_opt_leasetime_minutes=1440; // just to have a reasonable value: 1 day
                                break;
                        }
                        i=0;
                        while(i<4){
                                ltime_int32=(ltime_int32<<8) | buf[option_idx+i+2];
                                i++;
                        }
                        if (ltime_int32==0xffffffff){
                                // lease time is infinity
                                dhcp_opt_leasetime_minutes=0xffff;
                                break; // end of switch
                        }
                        ltime_int32=ltime_int32>>6; // an inexpesive way to divide by 64 (which is roughly equal to divide by 60)
                        if (ltime_int32> 0xfffd){ //0xffff is not handeled here because of the above break
                                dhcp_opt_leasetime_minutes=0xfffd;
                        }else{
                                dhcp_opt_leasetime_minutes=ltime_int32 & 0xffff;
                        }
                        if (dhcp_opt_leasetime_minutes<5) dhcp_opt_leasetime_minutes=5;
                        break;
                case 53: dhcp_opt_message_type=buf[option_idx+2];
                        break;
                        // rfc 2131: A DHCP server always returns its 
                        // own address in the 'server identifier' option
                case 54: if (option_len==4){
                                i=0;
                                while(i<4){
                                        dhcp_opt_server_id[i]=buf[option_idx+2+i];
                                        i++;
                                }
                        }
                        break;
                }
                option_idx+=2+option_len;
        }
        return(1);
}

// the answer to this message will come as a broadcast
uint8_t send_dhcp_request(uint8_t *buf,const uint8_t transactionID)
{
        uint8_t i=0;
        make_dhcp_message_template(buf,transactionID);
        // option dhcp message type:
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET]=0x35; // 53
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+1]=1; //len
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+2]=DHCP_REQUEST_V;
        i=3;
        if (dhcp_opt_server_id[0]!=0){
                buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i]=0x36; // 54=server identifier
                buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+1]=4; // len
                memcpy(buf+UDP_DATA_P+DHCP_OPTION_OFFSET+i+2,dhcp_opt_server_id, 4);
                i=i+6;
        }
        if (dhcp_yiaddr[0]!=0){
                buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i]=0x32; // 50=requested IP address
                buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+1]=4; // len
                memcpy(buf+UDP_DATA_P+DHCP_OPTION_OFFSET+i+2,dhcp_yiaddr, 4);
                i=i+6;
        }
        // option paramter request list:
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i]=0x37; // 55
        // we want: subnet mask, router
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+1]=2; // len
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+2]=1; // subnet mask
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+3]=3; // router=default GW
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+4]=0xff; // end of options
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+i+5]=0; 
        // the length of the udp message part is now DHCP_OPTION_OFFSET+i+5
        send_udp_transmit(buf,DHCP_OPTION_OFFSET+i+5);
        return(0);
}

// The renew procedure is described in rfc2131. 
// We send DHCPREQUEST and 'server identifier' MUST NOT be filled 
// in, 'requested IP address' option MUST NOT be filled in, 'ciaddr' 
// MUST be filled. 
// The rfc suggest that I can send the DHCPREQUEST in this case as
// a unicast message and not as a broadcast message but test with
// various DHCP servers show that not all of them listen to
// unicast. We send therefor a broadcast message but we expect
// a unicast answer directly to our mac and IP.
uint8_t send_dhcp_renew_request(uint8_t *buf,const uint8_t transactionID,uint8_t *yiaddr)
{
        make_dhcp_message_template(buf,transactionID);
        buf[UDP_DATA_P+4]=2; // set first byte in transaction ID to 2 to indicate renew_request. This trick makes the processing of the DHCPACK message easier.
        // source IP must be my IP since we renew
        memcpy(buf+IP_SRC_P, yiaddr, 4); // ip level source IP
        //
        memcpy(buf+UDP_DATA_P+12,yiaddr, 4); // ciaddr
        // option dhcp message type:
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET]=0x35; // 53
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+1]=1; //len
        buf[UDP_DATA_P+DHCP_OPTION_OFFSET+2]=DHCP_REQUEST_V;
        // no option paramter request list is needed at renew
        send_udp_transmit(buf,DHCP_OPTION_OFFSET+3);
        // we will get a unicast answer, reception of broadcast packets is turned off
        return(0);
}


// Initial_tid can be a random number for every board. E.g the last digit
// of the mac address. It is not so important that the number is random.
// It is more important that it is unique and no other board on the same
// Lan has the same number. This is because there might be a power outage
// and all boards reboot afterwards at the same time. At that moment they
// must all have different TIDs otherwise there will be an IP address mess-up.
//
// The function returns 1 once we have a valid IP. 
// At this point you must not call the function again.
uint8_t packetloop_dhcp_initial_ip_assignment(uint8_t *buf,uint16_t plen,uint8_t initial_tid){
        static uint16_t init=0x5fff; // about 2 sec delay
        uint8_t cmd;
        // do nothing if the link is down
        if (!enc28j60linkup()) return(0);
        // first time that this function is called:
        if (plen==0){
                if (init==2){
                        init=1;
                        dhcp_6sec_cnt=0;
                        dhcp_tid=initial_tid;
                        // Reception of broadcast packets is turned off by default, but
                        // the DHCP offer message that the DHCP server sends will be
                        // a broadcast packet. Enable here and disable later.
                        enc28j60EnableBroadcast();
                        send_dhcp_discover(buf,dhcp_tid);
                        return(0);
                }
                if (dhcp_yiaddr[0]==0 && dhcp_6sec_cnt > 5){
                        // still no IP after 30 sec
                        dhcp_tid++;
                        dhcp_6sec_cnt=0;
                        // Reception of broadcast packets is turned off by default, but
                        // the DHCP offer message that the DHCP server sends will be
                        // a broadcast packet. Enable here and disable later.
                        enc28j60EnableBroadcast();
                        send_dhcp_discover(buf,dhcp_tid);
                        return(0);
                }
                // We have a little delay (about 2sec) at startup. Sometimes
                // the power fluctuates or the programmer causes
                // the board to reset and then immediately reset again.
                // We wait with the sending of a send_dhcp_discover just in case
                // we did already so at the last reset which was possibly less
                // than a second ago.
                if (init>2){
                        init--;
                }
                return(0);
        }
        // plen > 0
        if (is_dhcp_msg_for_me(buf,plen,dhcp_tid)){
                // It's really a borderline case that we the the dhcp_is_renew_tid
                // function call for. It could only happen if the board is power cyceled 
                // during operation.
                if (dhcp_is_renew_tid(buf,plen)==1) return(0); // should have been initial tid, just return
                cmd=dhcp_get_message_type(buf,plen);
                if (cmd==2){ // DHCPOFFER =2
                        init=1; // no more init needed
                        dhcp_get_yiaddr(buf,plen);
                        dhcp_option_parser(buf,plen);
                        // answer offer with a request:
                        send_dhcp_request(buf,dhcp_tid);
                }
                if (cmd==5){ // DHCPACK =5
                        // success, DHCPACK, we have the IP
                        init=1; // no more init needed
                        enc28j60DisableBroadcast();
                        return(1);
                }
        }
        return(0);
}

// call this to get the current IP 
// returns {0,0,0,0} in assigend_yiaddr if called before we have a valid IP been offered
// otherwise returns back the IP address (4bytes) in assigend_yiaddr.
// assigend_netmask will hold the netmask and assigend_gw the default gateway
// You can fill fields that you don't want (not interessted in) to NULL
void dhcp_get_my_ip(uint8_t *assigend_yiaddr,uint8_t *assigend_netmask, uint8_t *assigend_gw){
        if (assigend_yiaddr) memcpy(assigend_yiaddr,dhcp_yiaddr,4); 
        if (assigend_netmask) memcpy(assigend_netmask,dhcp_opt_mask,4); 
        if (assigend_gw) memcpy(assigend_gw,dhcp_opt_defaultgw,4); 
}

// Put the following function into your main packet loop.
// returns plen of original packet if buf is not touched.
// returns 0 if plen was originally zero. returns 0 if DHCP messages
// was processed.
// We don't need to expect changing IP addresses. We can stick
// to the IP that we got once. The server has really no power to
// do anything about that.
uint16_t packetloop_dhcp_renewhandler(uint8_t *buf,uint16_t plen){
        if (dhcp_6sec_cnt> 8){  // we let it run a bit faster than once every minute because it is better this expires too early than too late
                dhcp_6sec_cnt=0;
                // count down unless the lease was infinite
                if (dhcp_opt_leasetime_minutes < 0xffff && dhcp_opt_leasetime_minutes>1){
                        dhcp_opt_leasetime_minutes--;
                }
        }
        if (plen ==0 && dhcp_opt_leasetime_minutes < 3){
                if (!enc28j60linkup()) return(plen); // do nothing if link is down
                dhcp_tid++;
                send_dhcp_renew_request(buf,dhcp_tid,dhcp_yiaddr);
                dhcp_opt_leasetime_minutes=5; // repeat in two minutes if no answer
                return(0);
        }
        if (plen && is_dhcp_msg_for_me(buf,plen,dhcp_tid)){
                // we check the dhcp_renew_tid because if 
                if (dhcp_get_message_type(buf,plen)==5){ // DHCPACK =5
                        // success, DHCPACK, we have the IP
                        if (dhcp_is_renew_tid(buf,plen)){
                                dhcp_option_parser(buf,plen); // get new lease time, it will as well GW and netmask but those should not change
                        }
                }
                return(0);
        }
        return(plen);
}

// === end of DHCP client
