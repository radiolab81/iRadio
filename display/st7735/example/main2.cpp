/*****************************************************************************************************
* TFT1_8 1,8" Display from Sainsmart with a ST7735 controller and a RaspberryPi
*
* This is a test program to control the 1.8" ST7735 TFT with the RaspberryPi and tft_st7735 library along
* with the wiringPi library
*/

#include "wiringPi.h"
#include "wiringPiSPI.h"

#include "tft_st7735.h"
#include "tft_manager.h"
#include "tft_field.h"

void testColors(TFT_ST7735& tft);

int main (void)
{
TFT_ST7735 tft = *new TFT_ST7735(0, 24, 25, 32000000);

  wiringPiSetupGpio();      // initialize wiringPi and wiringPiGpio

  tft.commonInit();         // initialize SPI and reset display
  tft.initR();              // initialize display
  tft.setRotation(true);
  tft.setBackground(TFT_BLACK);
  tft.clearScreen();        // reset Display

// draw lines
  tft.setBackground(TFT_BLACK);
  tft.clearScreen();        // reset Display
  tft.drawHorizontalLine(10, 10, 140, 0x01ff);
  tft.drawVerticalLine(10, 10, 140, 0xff00);
  tft.drawLine(10, 10, 150, 150, 0x8F80);

  delay (2000);
  tft.clearScreen();        // reset Display

// draw some text
  tft.drawString(0, 0,  "Hallo Size1", TFT_WHITE, 1);
  tft.drawString(0, 20, "Hallo Size2", TFT_YELLOW, 2);
  tft.drawString(0, 80, "Hallo Size3", TFT_GREEN, 3);

  delay (2000);
  tft.clearScreen();        // reset Display

// draw ASCII character set
   tft.setCursor(0,0);
   for (unsigned int i=1; i<=255; i++)
   {
     tft.drawChar((char)i, 0xffff, 1);
   }

  delay (2000);
  tft.clearScreen();        // reset Display

// draw rectangles
  tft.drawRect(10,10,100,50,TFT_YELLOW);
  tft.fillRect(10,70,100,50,TFT_PURPLE);

  delay (2000);
  tft.clearScreen();        // reset Display

// draw circles
  tft.drawCircle(30,30,10,TFT_YELLOW);
  tft.fillCircle(60,60,20,TFT_PURPLE);

  delay (2000);
  tft.clearScreen();        // reset Display

// draw some background colors
  testColors(tft);

// draw done
  tft.clearScreen();
  tft.drawString(40,(TFT_height/2)-10,"Done",TFT_WHITE,2);

  delay (2000);
  tft.clearScreen();        // reset Display

  return 0;
}


void testColors(TFT_ST7735& tft)
{
  tft.setBackground(TFT_BLACK);
  tft.clearScreen();
  tft.drawString(0,0,"Black",TFT_WHITE,2);
  delay(2000);
  tft.setBackground(TFT_GRAY);
  tft.clearScreen();
  tft.drawString(0,0,"Gray",TFT_BLACK,2);
  delay(2000);
  tft.setBackground(TFT_WHITE);
  tft.clearScreen();
  tft.drawString(0,0,"White",TFT_BLACK,2);
  delay(2000);
  tft.setBackground(TFT_RED);
  tft.clearScreen();
  tft.drawString(0,0,"RED",TFT_WHITE,2);
  delay(2000);
  tft.setBackground(TFT_GREEN);
  tft.clearScreen();
  tft.drawString(0,0,"Green",TFT_BLACK,2);
  delay(2000);
  tft.setBackground(TFT_BLUE);
  tft.clearScreen();
  tft.drawString(0,0,"Blue",TFT_WHITE,2);
  delay(2000);
  tft.setBackground(TFT_YELLOW);
  tft.clearScreen();
  tft.drawString(0,0,"Yellow",TFT_BLACK,2);
  delay(2000);
  tft.setBackground(TFT_ORANGE);
  tft.clearScreen();
  tft.drawString(0,0,"Orange",TFT_BLACK,2);
  delay(2000);
  tft.setBackground(TFT_PURPLE);
  tft.clearScreen();
  tft.drawString(0,0,"Purple",TFT_WHITE,2);
  delay(2000);
  tft.setBackground(TFT_BLACK);
}
