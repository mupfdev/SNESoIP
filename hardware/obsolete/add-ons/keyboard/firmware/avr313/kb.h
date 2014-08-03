// kb.h -*-c-*-
// Atmel application note AVR313.


#ifndef KB_h
#define KB_h

#define KbBufferSize 64


#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "../config.h"
#include "gpr.h"
#include "scancodes.h"


volatile uint8_t  kbBuffer[KbBufferSize];
volatile uint8_t *kbInPtr;
volatile uint8_t *kbOutPtr;
volatile uint8_t  kbBuffCnt;


void initKeyboard(void);
void decode(unsigned char sc);
void putKbBuff(unsigned char c);
int  getKbChar(void);


#endif
