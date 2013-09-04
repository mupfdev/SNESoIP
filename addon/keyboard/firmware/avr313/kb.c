// kc.c -*-c-*-
// Atmel application note AVR313.


#include "kb.h"


void initKeyboard(void) {
	kbInPtr   =  kbBuffer;  // Initialize buffer.
	kbOutPtr  = kbBuffer;
	kbBuffCnt = 0;

	MCUCR     = (1 << ISC01); // INT0 interrupt on falling edge.
	GIMSK     = (1 << INT0);    // Enable INT0 interrupt.
}


ISR (INT0_vect) {
	static uint8_t data = 0;            // Holds the received scan code
	static uint8_t bitCount = 11;       // 0 = neg.  1 = pos.

	if (bitCount < 11 && bitCount > 2) { // Bit 3 to 10 is data. Parity bit,

		// Start and stop bits are ignored.
		data = (data >> 1);
		if (KeyboardDataPIN & (1 << KeyboardData))
			data = data | 0x80;             // Store a '1'
	}

	if (--bitCount == 0) {               // All bits received
		bitCount = 11;
		decode(data);
	}
}


void decode(uint8_t sc) {
	static uint8_t isUp = 0, mode = 0;
	static uint8_t shift = 0;
	uint8_t i, ch;

	if (!isUp) {                         // Last data received was the up-key identifier
		if (sc == 0xF0)                    // The up-key identifier
			isUp = 1;

		else if (sc == 0x12 || sc == 0x59) // Left SHIFT or Right SHIFT
			shift = 1;

		else if (sc == 0x05) {             // F1
			if (mode == 0)
				mode = 1;                      // Enter scan code mode
			if(mode == 2)
				mode = 3;                      // Leave scan code mode

		} else {

			if (mode == 0 || mode == 3) {    // If ASCII mode
				if (!shift) {                  // If shift not pressed,

					// do a table look-up.
					for (i = 0; (ch = pgm_read_byte(&unshifted[i][0])) != sc && ch; i++);
					if (ch == sc)
						putKbBuff(pgm_read_byte(&unshifted[i][1]));

				} else {                      // If shift pressed.

					for (i = 0; (ch = pgm_read_byte(&shifted[i][0])) != sc && ch; i++);
					if (ch == sc)
						putKbBuff(pgm_read_byte(&shifted[i][1]));
				}
			} else {                        // Scan code mode.

				printHexByte(sc);             // Print scan code.
				putKbBuff(' ');
				putKbBuff(' ');
			}
		}
	} else {

		isUp = 0;                         // Two 0xF0 in a row not allowed.

		if (sc == 0x12 || sc == 0x59)      // Left SHIFT or Right SHIFT.
			shift = 0;
		else if (sc == 0x05) {             // F1.
			if (mode == 1)
				mode = 2;
			if (mode == 3)
				mode = 0;
		}
	}
}


// Stuff a decoded byte into the keyboard buffer.
// This routine is currently only called by "decode" which is called from
// within the ISR so atomic precautions are not needed here.
void putKbBuff(uint8_t c) {
	if (kbBuffCnt < KbBufferSize)	{ // If buffer not full
		// Put character into buffer.
		// Increment pointer.
		*kbInPtr++ = c;
		kbBuffCnt++;

		// Pointer wrapping.
		if (kbInPtr >= kbBuffer + KbBufferSize)
			kbInPtr = kbBuffer;
	}
}


// Get a char from the keyboard buffer.
// Routine does not return until a character is ready!
int getKbChar(void) {
	int byte;

	// Wait for data.
	while (kbBuffCnt == 0);

	uint8_t tmp = SREG;
	cli();

	// Get byte - Increment pointer
	byte = *kbOutPtr++;

	// Pointer wrapping
	if (kbOutPtr >= kbBuffer + KbBufferSize)
		kbOutPtr = kbBuffer;

	// Decrement buffer count
	kbBuffCnt--;

	SREG = tmp;

	return byte;
}
