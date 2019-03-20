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
int buffer[3] = {0,0,0};
int counter   = -10;

TFT_ST7735 tft      = *new TFT_ST7735(0, 24, 25, 32000000);
TFT_manager manager = *new TFT_manager();
TFT_field label1    = *new TFT_field (tft, 0, 10,50,20,TFT_WHITE,  1, TFT_BLACK, false);
TFT_field label2    = *new TFT_field (tft, 0, 40,50,20,TFT_WHITE,  1, TFT_BLACK, false);
TFT_field label3    = *new TFT_field (tft, 0, 70,50,20,TFT_WHITE,  1, TFT_BLACK, false);
TFT_field f1        = *new TFT_field (tft, 50,10,50,20,TFT_RED,    1, TFT_GRAY,  true);
TFT_field f2        = *new TFT_field (tft, 50,40,50,20,TFT_YELLOW, 1, TFT_BLUE,  true);
TFT_field f3        = *new TFT_field (tft, 50,70,50,20,TFT_PURPLE, 1, TFT_GREEN, true);

  manager.add(&label1);
  manager.add(&label2);
  manager.add(&label3);
  manager.add(&f1);
  manager.add(&f2);
  manager.add(&f3);

  wiringPiSetupGpio();      // initialize wiringPi and wiringPiGpio

  tft.commonInit();         // initialize SPI and reset display
  tft.initR();              // initialize display
  tft.setRotation(true);
  tft.setBackground(TFT_BLACK);
  tft.clearScreen();        // reset Display

  label1.setValue("Value1:");
  label2.setValue("Value2:");
  label3.setValue("Value3:");

  while(1)
  {
    // shift values through buffers and insert new value
    buffer[2] = buffer[1];
    buffer[1] = buffer[0];
    buffer[0] = counter;

    // alternate border color
    if (counter%2 == 0)
      f1.setColor(TFT_GREEN);
    else
      f1.setColor(TFT_RED);
    // set values to the fields
    f1.setValue(buffer[0]);
    f2.setValue(buffer[1]);
    f3.setValue(buffer[2]);

    // refresh display
    manager.refresh();
    delay (500);
    counter++;
  }
}
