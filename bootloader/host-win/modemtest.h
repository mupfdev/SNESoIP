#ifndef MODEMTEST_H
#define MODEMTEST_H

/* 
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */


/*
 *  function prototypes
 */
extern void ErrorHandler(char *message, DWORD error);

extern int modemInit( 
    char    *comPort,
    int     baudRate,
    HANDLE  *comHandle );
    
extern int modemReadLine(
    HANDLE  comHandle,
    char    *data,
    int     *dataLen );
        
extern int modemWriteMsg(
    HANDLE  comHandle,
    char    *data,
    int     dataLen );
    
extern uint16_t modemReadChar(HANDLE  comHandle);
extern uint16_t uart_getc(int timeout);
extern void     uart_puts(const uint8_t *s );
extern void     uart_putc(uint8_t data);
extern void     uart_puts_P(const uint8_t *s );
extern void     uart_flush(void);

extern HANDLE comHandle;
extern volatile    uint8_t  seen_cr;

#endif  /* MODEMTEST_H */
