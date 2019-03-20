/*
 *  lcdpcf8574.cpp:
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

#include "lcdpcf8574.h"

// lcdpcf8574 constructor
lcdpcf8574::lcdpcf8574(int addr, int onetimeinit, int wait, int backlight) {
	if ((fd = wiringPiI2CSetup(addr)) < 0) {
		printf("error initializing I2C device PCF8574, %d", fd);
	}

	int displayshift = (LCD_CURSORMOVE | LCD_MOVERIGHT);
	int displaymode = (LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
	int displaycontrol = (LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);

	blFlag = backlight == 0 ? LCD_NOBACKLIGHT : LCD_BACKLIGHT;

	if (onetimeinit > 0) {
		wiringPiI2CWrite(fd, 0x20);
		lcdpcf8574::lcd_strobe();
		if (wait == 1)
			usleep(10000);
		lcdpcf8574::lcd_write(
				LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
	}

	lcdpcf8574::lcd_write(LCD_DISPLAYCONTROL | displaycontrol);
	lcdpcf8574::lcd_write(LCD_ENTRYMODESET | displaymode);

	lcdpcf8574::lcd_write(LCD_CLEARDISPLAY);
	lcdpcf8574::lcd_write(LCD_CURSORSHIFT | displayshift);
	lcdpcf8574::lcd_write(LCD_RETURNHOME);
}

// clocks EN to latch command
void lcdpcf8574::lcd_strobe() {
	wiringPiI2CWrite(fd, wiringPiI2CRead(fd) | EN | LCD_BACKLIGHT);
	wiringPiI2CWrite(fd, (wiringPiI2CRead(fd) | LCD_BACKLIGHT) & 0xFB);
}

//write data to lcd in 4 bit mode, 2 nibbles
//high nibble is sent first
void lcdpcf8574::lcd_write(int cmd) {
	// write high nibble
	wiringPiI2CWrite(fd, (cmd & 0xF0) | blFlag);
	wiringPiI2CRead(fd);
	lcdpcf8574::lcd_strobe();

	// then low nibble
	wiringPiI2CWrite(fd, (cmd << 4) | blFlag);
	wiringPiI2CRead(fd);
	lcdpcf8574::lcd_strobe();
	wiringPiI2CWrite(fd, blFlag);
}

//write a character to lcd (or character rom) 0x09: backlight | RS=DR
//works as expected
void lcdpcf8574::lcd_write_char(int charvalue){
	int controlFlag = blFlag | RS;

       	// write high nibble
	wiringPiI2CWrite(fd, (controlFlag | (charvalue & 0xF0)));
	lcdpcf8574::lcd_strobe();

	// write low nibble
	wiringPiI2CWrite(fd, (controlFlag | (charvalue << 4)));
	lcdpcf8574::lcd_strobe();
	wiringPiI2CWrite(fd, blFlag);
}

//put char function
void lcdpcf8574::lcd_putc(int c){
	lcdpcf8574::lcd_write_char(c);
}

// load custom characters
void lcdpcf8574::lcd_load_custom_font(int addr, unsigned int font[8]){
	lcdpcf8574::lcd_write(addr*8 + LCD_SETCGRAMADDR);
	int x;
	for(x=0;x<8;x++){
		lcdpcf8574::lcd_write_char(font[x]);
	}
}

void lcdpcf8574::_setDDRAMAddress(int line, int col){
	// we write to the Data Display RAM (DDRAM)
	if(line == 0)
		lcdpcf8574::lcd_write(LCD_SETDDRAMADDR | (0x00 + col));
	if(line == 1)
		lcdpcf8574::lcd_write(LCD_SETDDRAMADDR | (0x40 + col));
	if(line == 2)
		lcdpcf8574::lcd_write(LCD_SETDDRAMADDR | (0x14 + col));
	if(line == 3)
		lcdpcf8574::lcd_write(LCD_SETDDRAMADDR | (0x54 + col));
}

//put string function
void lcdpcf8574::lcd_puts(char string[20], int line, int col){
	lcdpcf8574::_setDDRAMAddress(line, col);
	int len = strlen(string);
	int i;
	for(i = 0; i < len; i++){
		lcdpcf8574::lcd_putc(string[i]);
	}
}

void lcdpcf8574::lcd_put_custom(int c, int line, int col){
	lcdpcf8574::_setDDRAMAddress(line, col);
	lcdpcf8574::lcd_putc(c);
}


//clear lcd and set to home
void lcdpcf8574::lcd_clear(){
	lcdpcf8574::lcd_write(LCD_CLEARDISPLAY);
	lcdpcf8574::lcd_write(LCD_RETURNHOME);
}

// load 8 custom fonts to all available CGRAM locations
void lcdpcf8574::loadcustomfonts(CustomFontsStruct *fonts) {
	int x;
	for (x = 0; x < 8; x++) {
		lcdpcf8574::lcd_load_custom_font(x, fonts->array[x]);
	}
}

void lcdpcf8574::loadcustomfont(CustomFontStruct *font, int addr){
	lcdpcf8574::lcd_load_custom_font(addr, font->array);
}
