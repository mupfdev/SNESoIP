/* io.h -*-c-*-
 * I/O handler.
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#ifndef IO_h
#define IO_h

#include <avr/io.h>
#include <util/delay.h>
#include "config.h"


#define Disabled    0x00
#define Enabled     0xff


// Doesn't work if used in a loop with sendOutput(). But useful to catch
// key combinations at startup.
#define KeyB        0
#define KeyY        1
#define KeySelect   2
#define KeyStart    3
#define KeyUp       4
#define KeyDown     5
#define KeyLeft     6
#define KeyRight    7
#define KeyA        8
#define KeyX        9
#define KeyL       10
#define KeyR       11

#define InputKey(input, key) !(input & (1 << key))


typedef uint16_t snesIO;


static uint8_t configMode   = Disabled;
static uint8_t switchedMode = Disabled;


void     initInput();
void     initOutput();

snesIO   recvInput();
void     sendOutput(snesIO port0, snesIO port1);

void     initLed();
void     ledOff();
void     ledOnGreen();
void     ledOnRed();
void     ledToggleGreen();
void     ledToggleRed();
void     ledSignal(uint8_t times);


#endif
