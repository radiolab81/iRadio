#ifndef TFT_FIELD_H
#define TFT_FIELD_H

#include <string>
#include "tft_st7735.h"

/**
 * TFT_field class.
 *
 * The class provides a field for the TFT to show text and values
 */
class TFT_field
{
  private:
    TFT_ST7735& _tft;
    bool _refresh;

    unsigned char _start_x;
    unsigned char _start_y;
    unsigned char _width;
    unsigned char _height;
    unsigned int  _color;
    unsigned char _size;
    unsigned int  _background;
    bool          _border;
    std::string   _text;

  public:
    /** @brief Constructor of the class
     *
     *  @param &tft reference to the TFT_ST7735 instance
     *  @param start_x upper left corner of the field
     *  @param start_y upper left corner of the field
     *  @param width width of the field
     *  @param height height of the field
     *  @param color color for text and border in 5-6-5 format ( see Color565(r,g,b) )
     *  @param size factor for the size (1 = standard)
     *  @param background color of the background in 5-6-5 format ( see Color565(r,g,b) )
     *  @param border true: draw border
     *
     *  @return should not return a value
     *
     */
    TFT_field(TFT_ST7735& tft, unsigned char start_x, unsigned char start_y, unsigned char width, unsigned char height,
                  unsigned int color, unsigned char size, unsigned int background, bool border);

    /** @brief Set value(string) to display in the field
     *
     *  @param text string to display in the field
     *
     *  @return void
     *
     */
    void setValue (std::string text);

    /** @brief Set value(int) to display in the field
     *
     *  @param number integer to display in the field
     *
     *  @return void
     *
     */
    void setValue (int number);

   /** @brief Set value(double) to display in the field
     *
     *  @param number double to display in the field
     *
     *  @return void
     *
     */
     void setValue (double number);

   /** @brief Set color for text and border
     *
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     *
     */
    void setColor(unsigned int color);

   /** @brief Set size for the text
     *
     *  @param size factor for the size (1 = standard)
     *
     *  @return void
     *
     */
    void setSize (unsigned char size);

   /** @brief Refresh the field on the display
     *
     *  @return void
     *
     */
    void refresh (void);
};

#endif // TFT_FIELD_H
