/****************************************************************
 File:    modemtest.c
 Author:  Peter Fleury <pfleury@gmx.ch>
 
 Test modem communication using win32

 This code demonstrates an extremely simple serial connection.
 
 Compiled using the freeware MinGW Compiler http://www.mingw.org/
 
 Compile commands:
 gcc  -Wall -c modemtest.c
 gcc  modemtest.o   -o modemtest
 
 http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_serial.asp
 
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <inttypes.h>
#include <time.h>
#include "modemtest.h"







/*************************************************************************
Function: uart_getc(timeout)  timeout is in millisec
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
uint16_t uart_getc(int timeout)
{    
    uint16_t data ;
    clock_t t = clock ();		// actual time
    
    do
    {
    if((data = modemReadChar( comHandle)) != UART_NO_DATA)
    {
        //printf("-%02x-",data);
        return (data & 0x00ff);   /* got a char */
    }
    //} while( (clock()-t)/CLOCKS_PER_SEC < timeout );
    } while( (clock()-t) < timeout );


    return (UART_NO_DATA);

}/* uart_getc */

/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none          
**************************************************************************/
void uart_putc(uint8_t data)
{
  modemWriteMsg(comHandle, &data, 1);
   //printf("%c",data);
}/* uart_putc */



/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
void uart_puts(const uint8_t *s )
{
    while (*s) 
      uart_putc(*s++);

}/* uart_puts */

/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
void uart_puts_P(const uint8_t *s )
{
    while (*s) 
      uart_putc(*s++);

}/* uart_puts */

void uart_flush(void)
{
   while(uart_getc(0) != UART_NO_DATA);
}


/************************************************************************
open comport, set baudrate and return handle to it
************************************************************************/
int modemInit(
    char    *comPort,
    int     baudRate,
    HANDLE  *comHandle )
{
	BOOL success;
	DCB dcb;
	COMMTIMEOUTS timeouts;

	/* Open the comm port. Can open COM, LPT, or \\\\.\\TELNET */
    *comHandle = CreateFile(comPort, 
		                    GENERIC_READ|GENERIC_WRITE, 
		                    0, 0, OPEN_EXISTING,
		                    FILE_ATTRIBUTE_NORMAL, 0);
	if (*comHandle == INVALID_HANDLE_VALUE)
		ErrorHandler("In CreateFile", GetLastError());

	/* Get the current settings of the COMM port */
	success = GetCommState(*comHandle, &dcb);
	if (!success) ErrorHandler("In GetCommState",GetLastError());

    /* Modify the baud rate, etc. */
	dcb.BaudRate = baudRate;
	dcb.ByteSize = 8;
	dcb.Parity   = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;

	/* Apply the new comm port settings */
	success = SetCommState(*comHandle, &dcb);
	if (!success) ErrorHandler("In SetCommState",GetLastError());

	/* Change the ReadIntervalTimeout so that          */
	/* ReadFile will return immediately (MAXDWORD) or  */
	/* not return until it has data (0)                */
	//timeouts.ReadIntervalTimeout         = 0;
	timeouts.ReadIntervalTimeout         = MAXDWORD;	
	timeouts.ReadTotalTimeoutMultiplier  = 0;
	timeouts.ReadTotalTimeoutConstant    = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant   = 0;
	SetCommTimeouts( *comHandle, &timeouts );

    return 0;
}


uint16_t modemReadChar(HANDLE  comHandle)
/* read char from modem */
{
	BOOL  success;
	DWORD numRead;
    char c;
    
    success = ReadFile(comHandle, &c, 1, &numRead, 0);
    if(numRead == 0)
    {
      return(UART_NO_DATA);
    }
    
	if(numRead == 1)
	{
       return(c);
    }
    else
    {
        ErrorHandler("In ReadFile", GetLastError());
        return(UART_FRAME_ERROR);   
    }    
}/* modemReadChar */
    
int modemWriteMsg(
    HANDLE  comHandle,
    char    *data,
    int     dataLen )
/* write data to modem */
{
    BOOL success;
	DWORD numWrite;
    
	success = WriteFile(comHandle, data, dataLen, &numWrite, 0);
	if (!success) 
		ErrorHandler("modemWriteMsg:WriteFile", GetLastError());

    return 0;
    
};/* modemWriteMsg */



void ErrorHandler(char *message, DWORD error)
{
    fprintf(stderr,"Message=%s Errno=%u\n", message, (unsigned int)error );
	ExitProcess(1);
}



