/* ascii.c -*-c-*-
 * ASCII text reader.
 * Author: Michael Fitzmayer
 *
 * This program is part of the SNESoIP project and has has been released
 * under the terms of a BSD-like license.  See the file LICENSE for
 * details. */


#include <snes.h>


extern char snesfont;
char asciiTable[170];


void clear();
void initASCII();


int main(void) {
	int  x = 0, y = 0;
	int  charIndex = 0;
	char currentChar[2];
	unsigned short data;


	consoleInit();
	consoleInitText(0, 0, &snesfont);
	setMode(BG_MODE1, 0);
	bgSetDisable(1);
	bgSetDisable(2);
	setBrightness(0xF);
	initASCII();


	while(1) {
		scanPads();
		data = padsCurrent(1);

		charIndex = 0;
		if (data & BIT(15)) charIndex = charIndex + 1;
		if (data & BIT(14)) charIndex = charIndex + 2;
		if (data & BIT(13)) charIndex = charIndex + 4;
		if (data & BIT(12)) charIndex = charIndex + 8;
		if (data & BIT(11)) charIndex = charIndex + 16;
		if (data & BIT(10)) charIndex = charIndex + 32;
		if (data & BIT(9))  charIndex = charIndex + 64;
		if (data & BIT(8))  charIndex = charIndex + 128;
		if (charIndex > 169) charIndex = 0;

		currentChar[0] = asciiTable[charIndex];
		currentChar[1] = '\0';
		if (currentChar[0] == 10) { x = -1; y++; }
		if (currentChar[0] != 0) {
			consoleDrawText(x, y, currentChar);
			WaitForVBlank();
			x++;
		}
		if (x > 31) { y = y + 1; x = 0; }
		if (y > 26) { y = 0; clear(); }
	}


	return 0;
}


void clear() {
	int x = 0, y = 0;

	for (snes_vblank_count = 0; snes_vblank_count < 100; WaitForVBlank());

	setFadeEffect(FADE_OUT);
	for (y = 0; y <= 26; y++)
		for (x = 0; x <= 31; x++)
			consoleDrawText(x, y, " ");

	WaitForVBlank();
	setFadeEffect(FADE_IN);
}


void initASCII() {
	int i;
	for (i = 0; i < 169; i++)
		asciiTable[i] = 0;

	asciiTable[10]  = 10;
	asciiTable[32]  = ' ';
	asciiTable[33]  = '!';
	asciiTable[34]  = '"';
	asciiTable[35]  = '#';
	asciiTable[36]  = '$';
	asciiTable[37]  = '%';
	asciiTable[38]  = '&';
	asciiTable[40]  = '(';
	asciiTable[41]  = ')';
	asciiTable[42]  = '*';
	asciiTable[43]  = '+';
	asciiTable[44]  = ',';
	asciiTable[45]  = '-';
	asciiTable[46]  = '.';
	asciiTable[47]  = '/';
	asciiTable[48]  = '0';
	asciiTable[49]  = '1';
	asciiTable[50]  = '2';
	asciiTable[51]  = '3';
	asciiTable[52]  = '4';
	asciiTable[53]  = '5';
	asciiTable[54]  = '6';
	asciiTable[55]  = '7';
	asciiTable[56]  = '8';
	asciiTable[57]  = '9';
	asciiTable[58]  = ':';
	asciiTable[59]  = ';';
	asciiTable[60]  = '<';
	asciiTable[61]  = '=';
	asciiTable[62]  = '>';
	asciiTable[63]  = '?';
	asciiTable[64]  = '@';
	asciiTable[65]  = 'A';
	asciiTable[66]  = 'B';
	asciiTable[67]  = 'C';
	asciiTable[68]  = 'D';
	asciiTable[69]  = 'E';
	asciiTable[70]  = 'F';
	asciiTable[71]  = 'G';
	asciiTable[72]  = 'H';
	asciiTable[73]  = 'I';
	asciiTable[74]  = 'J';
	asciiTable[75]  = 'K';
	asciiTable[76]  = 'L';
	asciiTable[77]  = 'M';
	asciiTable[78]  = 'N';
	asciiTable[79]  = 'O';
	asciiTable[80]  = 'P';
	asciiTable[81]  = 'Q';
	asciiTable[82]  = 'R';
	asciiTable[83]  = 'S';
	asciiTable[84]  = 'T';
	asciiTable[85]  = 'U';
	asciiTable[86]  = 'V';
	asciiTable[87]  = 'W';
	asciiTable[88]  = 'X';
	asciiTable[89]  = 'Y';
	asciiTable[90]  = 'Z';
	asciiTable[97]  = 'a';
	asciiTable[98]  = 'b';
	asciiTable[99]  = 'c';
	asciiTable[91]  = '[';
	asciiTable[92]  = '\\';
	asciiTable[93]  = ']';
	asciiTable[94]  = '^';
	asciiTable[95]  = '_';
	asciiTable[96]  = '`';
	asciiTable[100] = 'd';
	asciiTable[101] = 'e';
	asciiTable[102] = 'f';
	asciiTable[103] = 'g';
	asciiTable[104] = 'h';
	asciiTable[105] = 'i';
	asciiTable[106] = 'j';
	asciiTable[107] = 'k';
	asciiTable[108] = 'l';
	asciiTable[109] = 'm';
	asciiTable[110] = 'n';
	asciiTable[111] = 'o';
	asciiTable[112] = 'p';
	asciiTable[113] = 'q';
	asciiTable[114] = 'r';
	asciiTable[115] = 's';
	asciiTable[116] = 't';
	asciiTable[117] = 'u';
	asciiTable[118] = 'v';
	asciiTable[119] = 'w';
	asciiTable[120] = 'x';
	asciiTable[121] = 'y';
	asciiTable[122] = 'z';
	asciiTable[123] = '{';
	asciiTable[124] = '|';
	asciiTable[125] = '}';
	asciiTable[126] = '~';
	asciiTable[169] = '©';
}
