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

int main (void)
{
TFT_ST7735 tft = *new TFT_ST7735(0, 24, 25, 32000000);

  wiringPiSetupGpio();      // initialize wiringPi and wiringPiGpio

  tft.commonInit();         // initialize SPI and reset display
  tft.initR();              // initialize display
  tft.setBackground(TFT_BLACK);

  tft.clearScreen();        // reset Display
  tft.setRotation(false);
  tft.drawString(0,0,"false",TFT_WHITE,1);
  delay (2000);

  tft.clearScreen();        // reset Display
  tft.setRotation(true);
  tft.drawString(0,0,"true",TFT_WHITE,1);
  delay (2000);

  tft.clearScreen();        // reset Display
  tft.setRotation(DEGREE_0);
  tft.drawString(0,0,"DEGREE_0",TFT_WHITE,1);
  delay (2000);

  tft.clearScreen();        // reset Display
  tft.setRotation(DEGREE_90);
  tft.drawString(0,0,"DEGREE_90",TFT_WHITE,1);
  delay (2000);

  tft.clearScreen();        // reset Display
  tft.setRotation(DEGREE_180);
  tft.drawString(0,0,"DEGREE_180",TFT_WHITE,1);
  delay (2000);  tft.clearScreen();        // reset Display

  tft.setRotation(DEGREE_270);
  tft.drawString(0,0,"DEGREE_270",TFT_WHITE,1);
  delay (2000);

  return 0;
}


