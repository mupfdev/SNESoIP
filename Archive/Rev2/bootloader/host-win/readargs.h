#define ABOOL  	 1
#define ACHAR    2
#define AINT     3
#define ALONG    4
#define AHEX     5
#define ASTRING	 6	/* Stringargument */
#define APOINTER 7
#define ANAME	 8	/* String ohne Schalterzeichen */

extern int _argc;
extern char **_argv;

int readargs( int argtyp,  char c, void *argp );
