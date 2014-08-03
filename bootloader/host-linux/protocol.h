//*************************************************************************
//          protocol definitions
//-------------------------------------------------------------------------
#define  COMMAND     0xA5        // Command sequence start
#define  ESCAPE      COMMAND

#define  CONNECT     0xA6        // connection established
#define  BADCOMMAND  0xA7        // command not supported
#define  ANSWER      0xA8        // followed by length byte
#define  CONTINUE    0xA9
#define  SUCCESS     0xAA
#define  FAIL        0xAB

#define  ESC_SHIFT   0x80        // offset escape char

#define  PROGEND     0xFF        // A5, FF
#define  REVISION    0           // get bootloader revision
#define  BUFFSIZE    1           // get buffer size
#define  SIGNATURE   2           // get target signature
#define  USERFLASH   3           // get user flash size
#define  PROGRAM     4           // program flash
#define  START       5           // start application
#define  CHECK_CRC   6           // CRC o.k.
#define  VERIFY      7           // Verify

#define  MAXFLASH 0x40000        // max flash size (256kB)

//-------------------------------------------------------------------------
//          APICALL definitions
//-------------------------------------------------------------------------
#define  API_PROG_PAGE  0x81     // copy one Page from SRAM to Flash

#define  API_SUCCESS    0x80     // success
#define  API_ERR_FUNC   0xF0     // function not supported
#define  API_ERR_RANGE  0xF1     // address inside bootloader
#define  API_ERR_PAGE   0xF2     // address not page aligned
//-------------------------------------------------------------------------


/*

Bootloader Protokoll:

- Der AVR wird durch Power On in Reset gesetzt.

- nach etwa 65ms geht er für etwa 300ms in den Bootloader
  und startet dann die Application


1.
Aktivieren des Bootloaders:

Senden des Paßworts "Peda", 0xFF
Wiederholen, bis CONNECT empfangen

Senden COMMAND
Antwort: SUCCESS

Zur Baudratenerkennung muß das Paßwort eine Zeichenfolge beinhalten, in der
1*Bitzeit 0 gefolgt von 4*Bitzeit 0 enthalten ist.
Die Protokollzeichen (CONNECT) dürfen nicht enthalten sein.


2.
Erkennung 1-Drahtmodus:

Es wird geprüft, ob das 1. Zeichen des Paßworts zurück kommt (Echo).
Das CONNECT wird gesendet auf der Startflanke des 0xFF.

Protokoll im Eindrahtmodus:
In der PC-Software werden die gesendeten Bytes nach einem Kommando gezählt.
Erst wenn die gleiche Anzahl Bytes empfangen wurde, werden die nachfolgenden
Bytes als Antwort gewertet.
Davor werden die Bytes ignoriert (nur gezählt).


3.
Abfrage Bootloaderversion:

Senden: COMMAND, REVISION
Antwort: ANSWER, 0x03, Revision_high, Revission_low, SUCCESS


4.
Abfrage Puffergröße:

Senden: COMMAND, BUFFSIZE
Antwort: ANSWER, 0x03, Buffersize_high, buffersize_low, SUCCESS


5.
Abfrage Useflashgrö0e:

Senden: COMMAND, USERFLASH
Antwort: ANSWER, 0x04, Flash_high, Flash_mid, Flash_low, SUCCESS


6.
Programmieren:

Senden COMMAND, PROGRAM

Senden Bytes bis Buffersize erreicht
Antwort: CONTINUE
Wiederholen, bis alle Bytes programmiert

wenn Byte == ESCAPE: senden ESCAPE, (ESCAPE+ESC_SHIFT)
wenn Byte == 0x13: senden ESCAPE, (0x13+ESC_SHIFT)

Programmieren beenden: senden ESCAPE, ESC_SHIFT
Antwort: SUCCESS


7.
Verify

Senden COMMAND, VERIFY
Senden Bytes
wenn Byte == ESCAPE: senden ESCAPE, (ESCAPE+ESC_SHIFT)

Programmieren beenden: senden ESCAPE, ESC_SHIFT
Antwort: SUCCESS - Verify o.k.
           FAIL - Verfy failed


8.
Start Applikation

Senden COMMAND, START
Antwort: keine

9.
CRC-Check:

Senden: COMMAND, CHECK_CRC, CRC_low, CRC_high
Antwort: SUCCESS - CRC o.k.
           FAIL - CRC failed

Es wird die 16Bit-CRC mit dem Polynom 0xA001 ermittelt
Nach Verbindungsaufbau wird das Commando gesendet, aber nicht ausgewertet,
damit der AVR seine CRC auf 0 setzen kann.
Nach Abfrage der Informationen wird die CRC das 1.mal geprüft.
Wenn dann schon ein Fehler erfolgte, wird nicht programmiert
(altes Programm wird nicht zerstört).


10. Fehler

Unbekannte Kommandos werden mit BADCOMMAND beantwortet.
Z.B. wenn im ATtiny CRC oder Verify ausgeklammert sind.
**************************************************************************/

/* --- end of file --- */
