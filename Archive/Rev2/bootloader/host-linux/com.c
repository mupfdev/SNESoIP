/**
 * Bootloader um dem Mikrocontroller Bootloader von Peter Dannegger anzusteuern
 *
 * License: GPL
 *
 * @author Bernhard Michler, based on source of Andreas Butti
 */


/// Includes
#include <sys/ioctl.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/times.h>
#include <sys/ioctl.h>

#include "com.h"
#include "protocol.h"


typedef struct
{
    unsigned long   value;
    speed_t         constval;
} baudInfo_t;
static baudInfo_t baudrates[] = {
    {     50,     B50 },
    {     75,     B75 },
    {    110,    B110 },
    {    134,    B134 },
    {    150,    B150 },
    {    200,    B200 },
    {    300,    B300 },
    {    600,    B600 },
    {   1200,   B1200 },
    {   1800,   B1800 },
    {   2400,   B2400 },
    {   4800,   B4800 },
    {   9600,   B9600 },
    {  19200,  B19200 },
    {  38400,  B38400 },
    {  57600,  B57600 },
    { 115200, B115200 },
    { 230400, B230400 }
};




/// Attributes

// Old settings
struct termios oldtio;
// CRC checksum
unsigned int crc = 0;

int sendCount = 0;

int waitcount = 0;

// time in usec neded for transferring one byte
static long bytetime;

/// Prototypes
void calc_crc(unsigned char d);


/**
 * Get the baud-id from baudrate, return B0 if invalid
 */
speed_t get_baudid (unsigned long baud)
{
    int i;
    speed_t baudid = B0;

    for(i = 0; i < (sizeof (baudrates) / sizeof (baudInfo_t)); i++)
    {
        if (baudrates[i].value == baud)
        {
            baudid = baudrates[i].constval;
            break;
        }
    }

    return (baudid);
}

/**
 * Get the time needed for transferring one byte 8N1 from baud-id, return 0 if invalid
 */
long get_bytetime (speed_t baudid)
{
    int i;
    long btime = 0;

    for(i = 0; i < (sizeof (baudrates) / sizeof (baudInfo_t)); i++)
    {
        if (baudrates[i].constval == baudid)
        {
            btime = (1000000L * 10L / baudrates[i].value) + 1;
            break;
        }
    }

    return (btime);
}

/**
 * Set flag for one-wire local echo
 *
 */
void com_localecho ()
{
    sendCount = 1;
}


/**
 * Get status of device. It might happen that the device disappears (e.g.
 * due to a USB-serial unplug).
 *
 * @return 1 if device seems ok, 0 if it seems not available
 */
int get_device_status(int fd)
{
    struct termios t;

    if (fd < 0)
        return 0;

    return !tcgetattr(fd, &t);
}

/**
 * Opens com port
 *
 * @return true if successfull
 */
int com_open (const char * device, speed_t baud, int wait_bytetime)
{
    struct termios newtio;
    int fd;

    // Open the device
    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        return fd;
    }

    // Save old settings
    tcgetattr(fd, &oldtio);

    // Init memory
    memset(&newtio, 0x00 , sizeof(newtio));

    // settings flags
    newtio.c_cflag = CS8 | CLOCAL | CREAD; // | CSTOPB;
    newtio.c_iflag = IGNPAR | IGNBRK;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;

    cfmakeraw(&newtio);

    // Timeout in 100ms
    newtio.c_cc[VTIME] = 0;
    // read 1 character
    newtio.c_cc[VMIN] = 0;

    // Setting baudrate
    cfsetispeed(&newtio, baud);
    cfsetospeed(&newtio, baud);

    // Flushing buffer
    tcflush(fd, TCIOFLUSH);

    // aplying new configuration
    tcsetattr(fd, TCSANOW, &newtio);

    sendCount = 0;

    if (wait_bytetime)
    {
        // do not use tcdrain, instead wait the time...
        // time in usec needed for transferring one byte
        // multiplied by the number of bytetimes that should be waited
        bytetime = get_bytetime (baud) * wait_bytetime;
    }
    else
    {
        bytetime = 0;
    }

    return fd;
}

/**
 * Sets the DTR (Data Terminal Ready) on the com port
 */
void com_set_dtr(int fd, unsigned char on)
{
    int flags;

    if (ioctl (fd, TIOCMGET, &flags) < 0)
    {
        perror ("ERROR: could not reset, getting V24 line status failed");
        return;
    }
    if (on)
    {
        flags |=  TIOCM_DTR;
    }
    else
    {
        flags &= ~TIOCM_DTR;
    }
    if (ioctl (fd, TIOCMSET, &flags) < 0)
    {
        perror ("ERROR: could not reset, setting V24 line status failed");
    }
}


/**
 * Toggles the DTR (Data Terminal Ready) on the com port
 */
void com_toggle_dtr(int fd)
{
    int flags;

    if (ioctl (fd, TIOCMGET, &flags) < 0)
    {
        perror ("ERROR: could not reset, getting V24 line status failed");
        return;
    }
    if (flags & TIOCM_DTR)
    {
        flags &= ~TIOCM_DTR;
    }
    else
    {
        flags |=  TIOCM_DTR;
    }
    if (ioctl (fd, TIOCMSET, &flags) < 0)
    {
        perror ("ERROR: could not reset, setting V24 line status failed");
    }
}


/**
 * Make sure all is written out....
 */
void com_drain (int fd)
{
    if (bytetime)
    {
        usleep (bytetime * waitcount);
        waitcount = 0;
    }
    else
    {
        while ((tcdrain(fd) < 0) && (errno == EINTR));
    }
}

/**
 * Close com port and restore settings
 */
void com_close(int fd)
{
    com_drain(fd);

    // restore old settings
    tcsetattr(fd, TCSANOW, &oldtio);

    // close device
    close(fd);
}

/**
 * Receives one char or -1 if timeout
 * timeout in 10th of seconds
 */
int com_getc(int fd,
             int timeout)
{
    static long         ticks = 0;
    static struct tms   theTimes;
    char    c;
    clock_t t = times (&theTimes);

    if (ticks == 0)
        ticks = sysconf(_SC_CLK_TCK) / 10;

    do
    {
        if (!get_device_status(fd))
        {
            return COM_DISCONNECT;
        }
        if (read(fd, &c, 1) == 1)
        {
            if (sendCount > 1)
            {
                sendCount--;
                t = times (&theTimes);
                continue;
            }
            return (unsigned char)c;
        }
    } while ( ((times (&theTimes) - t )/ticks) < timeout );

    return COM_TIMEOUT;
}

/*****************************************************************************
 *
 *      Read from serial port
 *
 ****************************************************************************/
int com_read (int       fd,
              char      *pszIn,
              size_t    tLen)
{
    int iNrRead;

    do {
        if (!get_device_status(fd))
        {
            printf ("\nDevice disconnected!\n");
            return -1;
        }

        iNrRead = read (fd, pszIn, tLen);
#if 0
        int flags;
        /* Get the current line bits */
        ioctl(fd, TIOCMGET, &flags);
        printf("Flags are %o.\n", flags);
#endif
    } while ((iNrRead < 0) && (errno == EINTR));

    return (iNrRead);
}

/**
 * Sends one char
 */
void com_putc_fast(int           fd,
                   unsigned char c)
{
    if (sendCount)
    {
        if (sendCount > 1)
            com_getc(fd, 0);
        sendCount++;
    }
    waitcount++;

    while ((write(fd, &c, 1) < 0) && (errno == EINTR));

    calc_crc( c ); // calculate transmit CRC
}

void com_putc(int fd, unsigned char c)
{
    com_drain(fd);
    com_putc_fast (fd, c);
}


/**
 * Sending a command
 */
void sendcommand(int fd, unsigned char c)
{
    if (sendCount)
        sendCount = 1;
    com_putc(fd, COMMAND);
    com_putc(fd, c);
    com_drain(fd);
}


/**
 * Calculate the new CRC sum
 */
void calc_crc(unsigned char d)
{
    int i;

    crc ^= d;
    for( i = 8; i; i-- )
    {
        crc = (crc >> 1) ^ ((crc & 1) ? 0xA001 : 0 );
    }
}

