//*************************************************************************
//				Protocol definitions
//-------------------------------------------------------------------------
#define	COMMAND		0xA5		// Command sequence start
#define ESCAPE		COMMAND

#define	CONNECT		0xA6		// connection established
#define	BADCOMMAND	0xA7		// command not supported
#define	ANSWER		0xA8		// followed by length byte
#define	CONTINUE	0xA9
#define	SUCCESS		0xAA
#define	FAIL		0xAB

#define	ESC_SHIFT	0x80		// offset escape char
#define	PROGEND		ESC_SHIFT
//-------------------------------------------------------------------------
