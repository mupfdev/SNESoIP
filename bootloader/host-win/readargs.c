/***************** Lesen Kommandozeilenargumente *****************/
/* Schalterargumente: -Cxxx  /Cxxx  -C-  -C+
   Stringargumente:   text1  text2
   Gros- und Kleinschreibung der Schalter wird nicht unterschieden
   jeder Aufruf liefert den nÑchsten Parameter
   z.B. Programm -A- -B123 text1 -a+ -B456 Text2
   1.Aufruf: A = FALSE, B = 123 Text = text1
   2.Aufruf: A = TRUE,  B = 456 Text = Text2
   d.h. nach Stringabfrage text1 sind alle Argumente davor ungÅltig
   und das 1. danach gefundene Argument gÅltig
   (Namen mit '-' am Anfang sind mit '--' zu kennzeichnen)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "readargs.h"

int readargs( int argtyp,  char c, void *argp )
{
  static int nc = 0;
  int i, ai;
  long al;
  int argc = _argc;
  char **argv = _argv;
  for( i = nc; ++i < argc; )
    switch( argv[i][0] ){
      case '-':
      case '/':
        if( toupper( argv[i][1] ) != toupper( c ) )
          break;
        switch( argtyp ){
          case ABOOL:
            if( argv[i][2] == '-' )
              *((int *) argp) = 0;
            else
              *((int *) argp) = 1;
            return( 1 );
          case AINT:
	    if( sscanf( argv[i]+2, "%d", &ai ) != 1 )
              continue;
            *((int *) argp) = ai;
            return( 1 );
          case AHEX:
            if( sscanf( argv[i]+2, "%h", &ai ) != 1 )
              continue;
            *((int *) argp) = ai;
            return( 1 );
          case ALONG:
	    if( sscanf( argv[i]+2, "%ld", &al ) != 1 )
              continue;
            *((long *) argp) = al;
            return( 1 );
          case ASTRING:
            strcpy( argp, argv[i]+2 );
            return( 1 );
	  case APOINTER:
	    argp = argv[i]+2;
            return( 1 );
          default: continue;
	}
      default:
	if( argtyp == ANAME && i > nc ){
          nc = i;
          strcpy( argp, argv[nc] );
          return( 1 );
        }
    }
  return( 0 );
}
