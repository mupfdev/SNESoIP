/**
 * Bootloader um dem Mikrocontroller Bootloader von Peter Dannegger anzusteuern
 *
 * License: GPL
 *
 * @author Andreas Butti
 */

#ifndef COM_H_INCLUDED
#define COM_H_INCLUDED

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>


#define COM_TIMEOUT     -1
#define COM_DISCONNECT  -2

extern unsigned int crc;

/// Prototypes

/**
 * Set flag for one-wire local echo
 *
 */
void com_localecho ();

/**
 * Opens com port
 *
 * @return descriptor
 */
int com_open(const char * device, speed_t baud, int use_drain);

/**
 * Close com port and restore settings
 */
void com_close(int fd);

/**
 * Sends one char
 */
void com_putc_fast(int fd, unsigned char c);
void com_putc(int fd, unsigned char c);

/**
 * Receives one char or -1 if timeout
 */
int com_getc(int fd, int timeout);

/**
 * Read input string
 */
int com_read (int       fd,
              char      *pszIn,
              size_t    tLen);

/**
 * Sending a command
 */
void sendcommand(int fd, unsigned char c);

/**
 * Get the baud-id from baudrate, return B0 if invalid
 */
speed_t get_baudid (unsigned long baud);

/**
 * Sets the DTR (Data Terminal Ready) on the com port
 */
void com_set_dtr(int fd, unsigned char on);

/**
 * Toggles the DTR (Data Terminal Ready) on the com port
 */
void com_toggle_dtr(int fd);

void calc_crc(unsigned char d);

int get_device_status(int fd);

#endif //COM_H_INCLUDED
