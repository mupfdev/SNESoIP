/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright:LGPL V2
 * See http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html
 *
 * DHCP client functions
 * This code uses the UDP_client framework. You need to enable UDP_client in ip_config.h to use this.
 *
 * Chip type           : ATMEGA88/168/328/644 with ENC28J60
 *********************************************/
//@{
#ifndef DHCP_CLIENT_H
#define DHCP_CLIENT_H 1

// Lease time renewal and time keeping.
// you must call this function every 6 seconds (approx 6 sec it can be exact but does not have
// to be. If in doubt call it very 5 sec rather than every 7 seconds). It is save
// to do this from interrupt
extern void dhcp_6sec_tick(void);
// Initial_tid can be a random number for every board. E.g the last digit
// of the mac address. It is not so important that the number is random.
// It is more important that it is unique and no other board on the same
// Lan has the same number. This is because there might be a power outage
// and all boards reboot afterwards at the same time. At that moment they
// must all have different TIDs otherwise there will be an IP address mess-up.
//
// The function returns 1 once we have a valid IP. 
// At this point you must not call the function again.
extern uint8_t packetloop_dhcp_initial_ip_assignment(uint8_t *buf,uint16_t plen,uint8_t initial_tid);
// get IP, netmask and GW after a successful DHCP offer. You can set the parameters that you don't need to NULL (e.g a server will only need the IP).
extern void dhcp_get_my_ip(uint8_t *assigend_yiaddr,uint8_t *assigend_netmask, uint8_t *assigend_gw);
// Put the following function into your main packet loop.
// returns plen of original packet if buf is not touched.
// returns 0 if plen was originally zero. returns 0 if DHCP messages
// was processed.
extern uint16_t packetloop_dhcp_renewhandler(uint8_t *buf,uint16_t plen);
//
//=== below is now an example on how to use this interface
/*

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
// http://www.nongnu.org/avr-libc/changes-1.8.html:
#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "dhcp_client.h"
// global variables in your program :
//
// please modify the following line. mac has to be unique in your local 
// area network. You can not have the same numbers in two devices:
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x29};
static uint8_t myip[4] = {0,0,0,0};
static volatile uint8_t sec=0; // counts up to 6 and goes back to zero
#define BUFFER_SIZE 850
static uint8_t buf[BUFFER_SIZE+1];
//
// timer interrupt, called automatically every second
ISR(TIMER1_COMPA_vect){
        sec++;
        if (sec>5){
                sec=0;
                dhcp_6sec_tick();
        }
}

// Generate an interrup about ever 1s form the 12.5MHz system clock
// Since we have that 1024 prescaler we do not really generate a second
// (1.00000256000655361677s) 
void timer_init(void)
{
        // write high byte first for 16 bit register access: 
        TCNT1H=0;  // set counter to zero
        TCNT1L=0;
        // Mode 4 table 14-4 page 132. CTC mode and top in OCR1A
        // WGM13=0, WGM12=1, WGM11=0, WGM10=0
        TCCR1A=(0<<COM1B1)|(0<<COM1B0)|(0<<WGM11);
        TCCR1B=(1<<CS12)|(1<<CS10)|(1<<WGM12)|(0<<WGM13); // crystal clock/1024

        // At what value to cause interrupt. You can use this for calibration
        // of the clock. Theoretical value for 12.5MHz: 12207=0x2f and 0xaf
        OCR1AH=0x2f;
        OCR1AL=0xaf;
        // interrupt mask bit:
        TIMSK1 = (1 << OCIE1A);
}

//in your main() function you would put this:
//
int main(void){
        uint16_t plen;
        uint16_t dat_p;
        uint8_t rval;
// ...

        timer_init();
        sei(); // interrupt enable
        // DHCP handling. Get the initial IP
        rval=0;
        init_mac(mymac);
        while(rval==0){
                plen=enc28j60PacketReceive(BUFFER_SIZE, buf);
                buf[BUFFER_SIZE]='\0';
                rval=packetloop_dhcp_initial_ip_assignment(buf,plen,mymac[5]);
        }
        dhcp_get_my_ip(myip,NULL,NULL); // we just want the IP, as a web server we do technically not need mask and gateway. Those fiels are only needed for a client.
        
        // ... in the main loop your put:

        while(1){

                // handle ping and wait for a tcp packet
                plen=enc28j60PacketReceive(BUFFER_SIZE, buf);
                buf[BUFFER_SIZE]='\0'; // http is an ascii protocol. Make sure we have a string terminator.
                // DHCP renew IP if needed:
                plen=packetloop_dhcp_renewhandler(buf,plen); // for this to work you have to call dhcp_6sec_tick() every 6 sec
                dat_p=packetloop_icmp_tcp(buf,plen);

                // ... your program continues here in the main loop and
                // ... you can handle UDP or TCP (e.g http data).
         }
}
*/
//=== end of example on how to use this interface

#endif /* DHCP_CLIENT_H */
//@}
