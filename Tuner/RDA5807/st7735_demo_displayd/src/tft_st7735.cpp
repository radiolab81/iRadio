/*****************************************************************************************************
* TFT1_8 1,8" Display from Sainsmart with a ST7735 controller and a RaspberryPi
*
* This is an example how to control the 1.8" ST7735 TFTs with the RaspberryPi and the wiringPi library
*
* This source code is based on several examples from the internet, mostly for the arduino.
*
*
* v0.1 2014/03/16 - Initial release / jschick
*
* This file is part of tft_st7735
*
* tft_st7735 is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* tft_st7735 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with tft_st7735. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wiringPi.h"
#include "wiringPiSPI.h"

#include "tft_st7735.h"

extern unsigned char  font[];

/*
 * TFT_ST7735:
 *  Constructor of the class
 *********************************************************************************
 */
TFT_ST7735::TFT_ST7735(unsigned char channel, unsigned char rs, unsigned char rst, int speed)
{
  _channel = channel;       // SPI channel (0 or 1)
  _rs   = rs;               // Pin number for Data - Command
  _rst  = rst;              // Pin number for Reset

  _speed = speed;           // clock rate for SPI

  _background = TFT_BLACK;  // background color
  _cursor_x = 0;            // cursor position for drawString and drawChar
  _cursor_y = 0;            // cursor position for drawString and drawChar

}


/*
 * clearScreen:
 *  Clear the whole screen with the background color
 *********************************************************************************
 */
void TFT_ST7735::clearScreen(void)
{
  setAddrWindow(0, 0, TFT_width-1, TFT_height-1);
  bulkDrawColor(_background, (unsigned int)TFT_width*TFT_height);
}


/*
 * clearScreen:
 *  Clear the whole screen with the specified color
 *********************************************************************************
 */
void TFT_ST7735::clearScreen(unsigned int  color)
{
  setAddrWindow(0, 0, TFT_width-1, TFT_height-1);
  bulkDrawColor(color, (unsigned int)TFT_width*TFT_height);
}

/*
 * setBackground:
 *  Set the background color
 *********************************************************************************
 */
void TFT_ST7735::setBackground(unsigned int color)
{
  _background = color;
}

/*
 * setCursor:
 *  Set the cursor position to the specified coordinates.
 *  Will be used by the drawString and drawChar methods
 *********************************************************************************
 */
void TFT_ST7735::setCursor(unsigned char x, unsigned char y)
{
  _cursor_x = x;
  _cursor_y = y;
}

/*
 * drawString:
 *  Draw a string. Cursor positions will be used
 *********************************************************************************
 */
void TFT_ST7735::drawString(const char *c, unsigned int color, unsigned char size)
{
  while (c[0] != 0)
  {
    drawChar(c[0], color, size);
  }
}

/*
 * drawString:
 *  Draw a string at the specified position
 *********************************************************************************
 */
void TFT_ST7735::drawString(unsigned char x, unsigned char y, const char *c,
            unsigned int color, unsigned char size)
{
  while (c[0] != 0)
  {
    drawChar(x, y, c[0], color, size);
    x += (FONT_WIDTH + 1)*size;
    c++;
    if (x + FONT_WIDTH*size >= TFT_width)
    {
      y += (FONT_HEIGHT+3)*size;
      x = 0;
    }
  }
}
/*
 * drawChar:
 *  Draw a character. Cursor positions will be used
 *********************************************************************************
 */
void TFT_ST7735::drawChar(char c, unsigned int color, unsigned char size)
{
  if (size == 1)
  {
    bulkDrawChar(_cursor_x, _cursor_y, c, color);
  }
  else
  {
    for (unsigned char i =0; i<FONT_WIDTH; i++ )
    {
      unsigned char line = font[(c*FONT_WIDTH)+i];
      for (unsigned char j = 0; j<(FONT_HEIGHT+1); j++)
      {
        if (line & 0x1)
        {
          fillRect(_cursor_x+i*size, _cursor_y+j*size, size, size, color);
        }
        line >>= 1;
      }
    }
  }

  _cursor_x += (FONT_WIDTH + 1)*size;
  if (_cursor_x+FONT_WIDTH*size >= TFT_width)
  {
      _cursor_y += (FONT_HEIGHT+3)*size;
      _cursor_x = 0;
  }
}

/*
 * drawChar:
 *  Draw a character at the specified position
 *********************************************************************************
 */
void TFT_ST7735::drawChar(unsigned char x, unsigned char y, char c,
              unsigned int color, unsigned char size)
{
  if (size == 1)
  {
    bulkDrawChar(x, y, c, color);
  }
  else
  {
    for (unsigned char i =0; i<FONT_WIDTH; i++ )
    {
      unsigned char line = font[(c*FONT_WIDTH)+i];
      for (unsigned char j = 0; j<(FONT_HEIGHT+1); j++)
      {
        if (line & 0x1)
        {
          fillRect(x+i*size, y+j*size, size, size, color);
        }
        line >>= 1;
      }
    }
  }
}

/*
 * drawChar:
 *  Draw a character at the specified position. Bulk transfer will be used
 *********************************************************************************
 */
void TFT_ST7735::bulkDrawChar(unsigned char x, unsigned char y, char c,
              unsigned int color)
{
unsigned int values[FONT_WIDTH*(FONT_HEIGHT+1)];
unsigned char col1, col2;
unsigned char* p = (unsigned char*)values;

  for (unsigned char i =0; i<FONT_WIDTH; i++ )
  {
    unsigned char line = font[(c*FONT_WIDTH)+i];
    for (unsigned char j = 0; j<(FONT_HEIGHT+1); j++)
    {
      if (line & 0x1)
      {
        col1 = color>>8;
        col2 = color;
      }
      else
      {
        col1 = _background>>8;
        col2 = _background;
      }
      unsigned int offset = (2*(j*FONT_WIDTH) + 2*i);
      p[offset]   = col1;
      p[offset+1] = col2;
      line >>= 1;
    }
  }
  setAddrWindow(x, y, x+(FONT_WIDTH-1), y+((FONT_HEIGHT+1)-1));
  writedata((unsigned char*)values,2*FONT_WIDTH*(FONT_HEIGHT+1));
}

/*
 * drawVerticalLine:
 *  Draw a vertical line
 *********************************************************************************
 */
void TFT_ST7735::drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
  if (x >= TFT_width) return;
  if (y+length >= TFT_height)
    length = TFT_height-y-1;

  drawFastLine(x,y,length,color,1);
}

/*
 * drawHorizontalLine:
 *  Draw a horizontal line
 *********************************************************************************
 */
void TFT_ST7735::drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
  if (y >= TFT_height) return;
  if (x+length >= TFT_width)
    length = TFT_width-x-1;

  drawFastLine(x,y,length,color,0);
}

/*
 * drawFastLine:
 *  Draw a horizontal or vertical line with bulk transfer
 *********************************************************************************
 */
void TFT_ST7735::drawFastLine(unsigned char x, unsigned char y, unsigned char length,
              unsigned int color, unsigned char rotflag)
{
  if (rotflag)
  {
    setAddrWindow(x, y, x, y+length);
  }
  else
  {
    setAddrWindow(x, y, x+length, y+1);
  }

  bulkDrawColor(color, length);
}

/*
 * drawLineLine:
 *  Draw a none oriented line. Uses bresenham's algorithm - thx wikpedia
 *********************************************************************************
 */
void TFT_ST7735::drawLine(unsigned int x0, unsigned int y0, int x1, int y1, unsigned int color)
{
unsigned int steep = abs(int(y1 - y0)) > abs(int(x1 - x0));

  if (steep)
  {
    SWAP(x0, y0);
    SWAP(x1, y1);
  }

  if ((int)x0 > x1)
  {
    SWAP(x0, x1);
    SWAP(y0, y1);
  }

  unsigned int dx, dy;
  dx = x1 - x0;
  dy = abs(int(y1 - y0));

  int err = dx / 2;
  int ystep;

  if ((int)y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; (int)x0<=x1; x0++)
  {
    if (steep)
    {
      drawPixel(y0, x0, color);
    }
    else
    {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

/*
 * drawRect:
 *  Draw a rectangle
 *********************************************************************************
 */
void TFT_ST7735::drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,
              unsigned int color)
{
  // smarter version
  drawHorizontalLine(x, y, w, color);
  drawHorizontalLine(x, y+h-1, w, color);
  drawVerticalLine(x, y, h, color);
  drawVerticalLine(x+w-1, y, h, color);
}

/*
 * fillRect:
 *  Draw a filled rectangle
 *********************************************************************************
 */
void TFT_ST7735::fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,
              unsigned int color)
{
unsigned int values[TFT_width];
unsigned char col1 = color >> 8;
unsigned char col2 = color;

  setAddrWindow(x, y, x+w-1, y+h-1);

  for (unsigned int i=0; i < h; i++)
  {
    unsigned char* p = (unsigned char*)values;

    for (unsigned int j=0; j < w; j++)
    {
        *p++ = col1;
        *p++ = col2;
    }
    writedata((unsigned char*)values, w*2);
  }
}


/*
 * drawCircle:
 *  Draw a circle
 *********************************************************************************
 */
void TFT_ST7735::drawCircle(unsigned char x0, unsigned char y0, unsigned char r,
            unsigned int color)
{
int f = 1 - r;
int ddF_x = 1;
int ddF_y = -2 * r;
int x = 0;
int y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);

    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);

  }
}

/*
 * fillCircle:
 *  Draw a filled circle
 *********************************************************************************
 */
void TFT_ST7735::fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color)
{
int f = 1 - r;
int ddF_x = 1;
int ddF_y = -2 * r;
int x = 0;
int y = r;

  drawVerticalLine(x0, y0-r, 2*r+1, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawVerticalLine(x0+x, y0-y, 2*y+1, color);
    drawVerticalLine(x0-x, y0-y, 2*y+1, color);
    drawVerticalLine(x0+y, y0-x, 2*x+1, color);
    drawVerticalLine(x0-y, y0-x, 2*x+1, color);
  }
}


/*
 * setRotation:
 *  set rotation value
 *********************************************************************************
 */
void TFT_ST7735::setRotation(bool value)
{
  writecommand(ST7735_MADCTL);  // memory access control (directions)
  if (value)
    writedata(0x08);
  else
    writedata(0xc8);
}

/*
 * setRotation:
 *  set rotation value
 *********************************************************************************
 */
void TFT_ST7735::setRotation(rotate_E value)
{
  writecommand(ST7735_MADCTL);  // memory access control (directions)
  switch (value)
  {
    case DEGREE_0:
      writedata(0xc8);
      break;
    case DEGREE_90:
      writedata(0x68);
      break;
    case DEGREE_180:
      writedata(0x08);
      break;
    case DEGREE_270:
      writedata(0xa8);
      break;
    default:
      // do nothing
      break;
  }
}

/*
 * writecommand:
 *  write a command via SPI
 *********************************************************************************
 */
void TFT_ST7735::writecommand(unsigned char c)
{
unsigned char data = c;

  digitalWrite(_rs, LOW);

  if( wiringPiSPIDataRW (_channel, &data, 1) < 0)
  {
    printf("TFT_ST7735::writecommand error %s\n", strerror(errno) );
  }
}


/*
 * writedata:
 *  write data via SPI
 *********************************************************************************
 */
void TFT_ST7735::writedata(unsigned char *data, int len)
{
  digitalWrite(_rs, HIGH);

  if( wiringPiSPIDataRW (_channel, data, len) < 0)
  {
    printf("TFT_ST7735::writedata error %s\n", strerror(errno) );
  }
}

/*
 * writedata:
 *  write a single byte via SPI
 *********************************************************************************
 */
void TFT_ST7735::writedata(unsigned char data)
{
  writedata (&data, 1);
}

/*
 * setAddrWindow:
 *  set a window of the TFT which will be filled by the bulk transfer
 *********************************************************************************
 */
void TFT_ST7735::setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
  writecommand(ST7735_CASET);   // column addr set
  writedata(0x00);
  writedata(x0+0);              // XSTART
  writedata(0x00);
  writedata(x1+0);              // XEND

  writecommand(ST7735_RASET);   // row addr set
  writedata(0x00);
  writedata(y0+0);              // YSTART
  writedata(0x00);
  writedata(y1+0);              // YEND

  writecommand(ST7735_RAMWR);   // write to RAM
}

/*
 * bulkDrawColor:
 *  transfer bulk data via SPI
 *********************************************************************************
 */
void TFT_ST7735::bulkDrawColor (unsigned int color, unsigned int length)
{
unsigned int values[MAX_SPI_MSG_SIZE/2];
unsigned char col1 = color >> 8;
unsigned char col2 = color;
unsigned char* p = (unsigned char*)values;

  if (length > (TFT_width * TFT_height))
    return;

  while (length > 0)
  {
    p = (unsigned char*)values;

    if (length <= (MAX_SPI_MSG_SIZE / 2))
    {
      for (unsigned int i=0; i < length; i++)
      {
        *p++ = col1;
        *p++ = col2;
      }
      writedata((unsigned char*)values, length*2);
      length -= length;
    }
    else
    {
      for (unsigned int i=0; i < (MAX_SPI_MSG_SIZE / 2); i++)
      {
        *p++ = col1;
        *p++ = col2;
      }
      writedata((unsigned char*)values, MAX_SPI_MSG_SIZE);
      length -= (MAX_SPI_MSG_SIZE / 2);
    }
  }
}

/*
 * pushColor:
 *  transfer a single color value via SPI
 *********************************************************************************
 */
void TFT_ST7735::pushColor(unsigned int color)
{
  writedata(color >> 8);
  writedata(color);
}

/*
 * drawPixel:
 *  draw a single pixel
 *********************************************************************************
 */
void TFT_ST7735::drawPixel(unsigned char x, unsigned char y, unsigned int color)
{
  if ((x >= TFT_width) || (y >= TFT_height))
    return;

  setAddrWindow(x,y,x+1,y+1);

  // setup for data
  pushColor(color);

}

/*
 * commonInit:
 *  initialize SPI and reset display
 *********************************************************************************
 */
void TFT_ST7735::commonInit(void)
{
  pinMode(_rs, OUTPUT);

  // 0 is SPI0_CE0_N which is GPIO8 (first chip select line) => Channel 0
  if (wiringPiSPISetup (_channel, _speed) < 0)
  {
    // Handle error
    printf("TFT_ST7735::commonInit Error setting up SPI\n");
  }

  // toggle RST low to reset; CS low so it'll listen to us
  if (_rst)
  {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(500);
        digitalWrite(_rst, LOW);
        delay(500);
        digitalWrite(_rst, HIGH);
        delay(500);
  }
}

/*
 * initB:
 *  initialize display
 *********************************************************************************
 */
void TFT_ST7735::initB(void)
{

  writecommand(ST7735_SWRESET); // software reset
  delay(50);
  writecommand(ST7735_SLPOUT);  // out of sleep mode
  delay(500);

  writecommand(ST7735_COLMOD);  // set color mode
  writedata(0x05);        // 16-bit color
  delay(10);

  writecommand(ST7735_FRMCTR1);  // frame rate control
  writedata(0x00);  // fastest refresh
  writedata(0x06);  // 6 lines front porch
  writedata(0x03);  // 3 lines backporch
  delay(10);

  writecommand(ST7735_MADCTL);  // memory access control (directions)
  writedata(0xC8);              // row address/col address, bottom to top refresh

  writecommand(ST7735_DISSET5);  // display settings #5
  writedata(0x15);  // 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
  writedata(0x02);  // fix on VTL

  writecommand(ST7735_INVCTR);  // display inversion control
  writedata(0x0);  // line inversion

  writecommand(ST7735_PWCTR1);  // power control
  writedata(0x02);      // GVDD = 4.7V
  writedata(0x70);      // 1.0uA
  delay(10);
  writecommand(ST7735_PWCTR2);  // power control
  writedata(0x05);      // VGH = 14.7V, VGL = -7.35V
  writecommand(ST7735_PWCTR3);  // power control
  writedata(0x01);      // Opamp current small
  writedata(0x02);      // Boost frequency


  writecommand(ST7735_VMCTR1);  // power control
  writedata(0x3C);      // VCOMH = 4V
  writedata(0x38);      // VCOML = -1.1V
  delay(10);

  writecommand(ST7735_PWCTR6);  // power control
  writedata(0x11);
  writedata(0x15);

  writecommand(ST7735_GMCTRP1);
  writedata(0x0f);  //writedata(0x09);
  writedata(0x1a);  //writedata(0x16);
  writedata(0x0f);  //writedata(0x09);
  writedata(0x18);  //writedata(0x20);
  writedata(0x2f);  //writedata(0x21);
  writedata(0x28);  //writedata(0x1B);
  writedata(0x20);  //writedata(0x13);
  writedata(0x22);  //writedata(0x19);
  writedata(0x1f);  //writedata(0x17);
  writedata(0x1b);  //writedata(0x15);
  writedata(0x23);  //writedata(0x1E);
  writedata(0x37);  //writedata(0x2B);
  writedata(0x00);  //writedata(0x04);
  writedata(0x07);  //writedata(0x05);
  writedata(0x02);  //writedata(0x02);
  writedata(0x10);  //writedata(0x0E);
  writecommand(ST7735_GMCTRN1);
  writedata(0x0f);   //writedata(0x0B);
  writedata(0x1b);   //writedata(0x14);
  writedata(0x0f);   //writedata(0x08);
  writedata(0x17);   //writedata(0x1E);
  writedata(0x33);   //writedata(0x22);
  writedata(0x2c);   //writedata(0x1D);
  writedata(0x29);   //writedata(0x18);
  writedata(0x2e);   //writedata(0x1E);
  writedata(0x30);   //writedata(0x1B);
  writedata(0x30);   //writedata(0x1A);
  writedata(0x39);   //writedata(0x24);
  writedata(0x3f);   //writedata(0x2B);
  writedata(0x00);   //writedata(0x06);
  writedata(0x07);   //writedata(0x06);
  writedata(0x03);   //writedata(0x02);
  writedata(0x10);   //writedata(0x0F);
  delay(10);

  writecommand(ST7735_CASET);  // column addr set
  writedata(0x00);
  writedata(0x02);   // XSTART = 2
  writedata(0x00);
  writedata(0x81);   // XEND = 129

  writecommand(ST7735_RASET);  // row addr set
  writedata(0x00);
  writedata(0x02);    // XSTART = 1
  writedata(0x00);
  writedata(0x81);    // XEND = 160

  writecommand(ST7735_NORON);  // normal display on
  delay(10);

  writecommand(ST7735_RAMWR);
  delay(500);

  writecommand(ST7735_DISPON);
  delay(500);
}


/*
 * initR:
 *  initialize display
 *********************************************************************************
 */
void TFT_ST7735::initR(void)
{

  writecommand(ST7735_SWRESET); // software reset
  delay(150);

  writecommand(ST7735_SLPOUT);  // out of sleep mode
  delay(500);

  writecommand(ST7735_FRMCTR1);  // frame rate control - normal mode
  writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
  writedata(0x2C);
  writedata(0x2D);

  writecommand(ST7735_FRMCTR2);  // frame rate control - idle mode
  writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
  writedata(0x2C);
  writedata(0x2D);

  writecommand(ST7735_FRMCTR3);  // frame rate control - partial mode
  writedata(0x01); // dot inversion mode
  writedata(0x2C);
  writedata(0x2D);
  writedata(0x01); // line inversion mode
  writedata(0x2C);
  writedata(0x2D);

  writecommand(ST7735_INVCTR);  // display inversion control
  writedata(0x07);  // no inversion

  writecommand(ST7735_PWCTR1);  // power control
  writedata(0xA2);
  writedata(0x02);      // -4.6V
  writedata(0x84);      // AUTO mode

  writecommand(ST7735_PWCTR2);  // power control
  writedata(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

  writecommand(ST7735_PWCTR3);  // power control
  writedata(0x0A);      // Opamp current small
  writedata(0x00);      // Boost frequency

  writecommand(ST7735_PWCTR4);  // power control
  writedata(0x8A);      // BCLK/2, Opamp current small & Medium low
  writedata(0x2A);

  writecommand(ST7735_PWCTR5);  // power control
  writedata(0x8A);
  writedata(0xEE);

  writecommand(ST7735_VMCTR1);  // power control
  writedata(0x0E);

  writecommand(ST7735_INVOFF);    // don't invert display

  writecommand(ST7735_MADCTL);  // memory access control (directions)
  writedata(0xC8);              // row address/col address, bottom to top refresh

  writecommand(ST7735_COLMOD);  // set color mode
  writedata(0x05);        // 16-bit color

  writecommand(ST7735_CASET);  // column addr set
  writedata(0x00);
  writedata(0x00);   // XSTART = 0
  writedata(0x00);
  writedata(0x7F);   // XEND = 127

  writecommand(ST7735_RASET);  // row addr set
  writedata(0x00);
  writedata(0x00);    // XSTART = 0
  writedata(0x00);
  writedata(0x9F);    // XEND = 159

  writecommand(ST7735_GMCTRP1);
  writedata(0x0f);
  writedata(0x1a);
  writedata(0x0f);
  writedata(0x18);
  writedata(0x2f);
  writedata(0x28);
  writedata(0x20);
  writedata(0x22);
  writedata(0x1f);
  writedata(0x1b);
  writedata(0x23);
  writedata(0x37);
  writedata(0x00);
  writedata(0x07);
  writedata(0x02);
  writedata(0x10);
  writecommand(ST7735_GMCTRN1);
  writedata(0x0f);
  writedata(0x1b);
  writedata(0x0f);
  writedata(0x17);
  writedata(0x33);
  writedata(0x2c);
  writedata(0x29);
  writedata(0x2e);
  writedata(0x30);
  writedata(0x30);
  writedata(0x39);
  writedata(0x3f);
  writedata(0x00);
  writedata(0x07);
  writedata(0x03);
  writedata(0x10);

  writecommand(ST7735_DISPON);
  delay(100);

  writecommand(ST7735_NORON);  // normal display on
  delay(10);
}
