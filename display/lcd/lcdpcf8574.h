/*
 *  lcdpcf8574.h:
 *  I²C based PCF8574 driver for HD44780 based LCD to be used with wiringPi
 *  for Raspberry Pi
 *
 * Copyright (c) 2013 Prageeth Karunadheera. http://karunadheera.com/
 ***********************************************************************
 *
 *    lcdpcf8574 is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    lcdpcf8574 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <wiringPiI2C.h>

#ifndef LCDPCF8574_H
#define LCDPCF8574_H

// initializes objects and lcd

// LCD Commands
unsigned int LCD_CLEARDISPLAY = 0x01;
unsigned int LCD_RETURNHOME = 0x02;
unsigned int LCD_ENTRYMODESET = 0x04;
unsigned int LCD_DISPLAYCONTROL = 0x08;
unsigned int LCD_CURSORSHIFT = 0x10;
unsigned int LCD_FUNCTIONSET = 0x20;
unsigned int LCD_SETCGRAMADDR = 0x40;
unsigned int LCD_SETDDRAMADDR = 0x80;

// Flags for display on/off control
unsigned int LCD_DISPLAYON = 0x04;
unsigned int LCD_DISPLAYOFF = 0x00;
unsigned int LCD_CURSORON = 0x02;
unsigned int LCD_CURSOROFF = 0x00;
unsigned int LCD_BLINKON = 0x01;
unsigned int LCD_BLINKOFF = 0x00;

// Flags for display entry mode
unsigned int LCD_ENTRYRIGHT = 0x00;
unsigned int LCD_ENTRYLEFT = 0x02;
unsigned int LCD_ENTRYSHIFTINCREMENT = 0x01;
unsigned int LCD_ENTRYSHIFTDECREMENT = 0x00;

// Flags for display/cursor shift
unsigned int LCD_DISPLAYMOVE = 0x08;
unsigned int LCD_CURSORMOVE = 0x00;
unsigned int LCD_MOVERIGHT = 0x04;
unsigned int LCD_MOVELEFT = 0x00;

// flags for function set
unsigned int LCD_8BITMODE = 0x10;
unsigned int LCD_4BITMODE = 0x00;
unsigned int LCD_2LINE = 0x08;
unsigned int LCD_1LINE = 0x00;
unsigned int LCD_5x10DOTS = 0x04;
unsigned int LCD_5x8DOTS = 0x00;

// flags for backlight control
// invert backlight flag since LEDs are common anode
unsigned int LCD_BACKLIGHT = 0x00;
unsigned int LCD_NOBACKLIGHT = 0x08;

unsigned int EN = 0x04;  // Enable bit
unsigned int RW = 0x02;  // Read/Write bit
unsigned int RS = 0x01;  // Register select bit

int blFlag;

/*
pinout for 20x4 LCD via PCF8574AN:
----------
0x80    P7 -  - D7
0x40    P6 -  - D6
0x20    P5 -  - D5
0x10    P4 -  - D4
-----------
0x08    P3 -  - BL   Backlight (LCD has 3 RGB LEDs - common anode)
0x04    P2 -  - EN   Starts Data read/write
0x02    P1 -  - RW   low: write, high: read
0x01    P0 -  - RS   Register Select: 0: Instruction Register (IR) (AC when read), 1: data register (DR)
*/


typedef struct CustomFontsStruct{
	unsigned int array[8][8];
}CustomFontsStruct;
typedef struct CustomFontStruct{
	unsigned int array[8];
}CustomFontStruct;

class lcdpcf8574 {
private:
	int fd;
	lcdpcf8574(){}; // private default constructor
public:
	lcdpcf8574(int addr, int onetimeinit, int wait, int backlight); // public constructor with address for I²C device
	void lcd_strobe();
	void lcd_write(int cmd);
	void lcd_load_custom_font(int addr, unsigned int font[8]);
	void lcd_write_char(int charvalue);
	void lcd_putc(int c);
	void _setDDRAMAddress(int line, int col);
	void lcd_puts(char string[20], int line, int col);
	void lcd_put_custom(int c, int line, int col);
	void lcd_clear();
	void loadcustomfonts(CustomFontsStruct *fonts);
	void loadcustomfont(CustomFontStruct *font, int addr);
};

#ifdef __cplusplus
extern "C" {
#endif

lcdpcf8574 *lcdpcf8574instance;

extern void new_lcdpcf8574(int addr, int onetimeinit, int wait, int backlight) {
	lcdpcf8574instance = new lcdpcf8574(addr, onetimeinit, wait, backlight);
}
extern void lcdpcf8574_loadcustomfonts(CustomFontsStruct *fonts) {
	lcdpcf8574instance->loadcustomfonts(fonts);
}
extern void lcdpcf8574_loadcustomfont(CustomFontStruct *font, int addr) {
	lcdpcf8574instance->loadcustomfont(font, addr);
}
extern void lcdpcf8574_lcd_puts(char string[20], int line, int col) {
	lcdpcf8574instance->lcd_puts(string, line, col);
}

extern void lcdpcf8574_lcd_put_custom(int c, int line, int col) {
	lcdpcf8574instance->lcd_put_custom(c, line, col);
}
extern void lcdpcf8574_lcd_clear() {
	lcdpcf8574instance->lcd_clear();
}

#ifdef __cplusplus
}
#endif

#endif
