/** @file */
#ifndef TFT1_8_H
#define TFT1_8_H

static const unsigned char TFT_width  = 128;
static const unsigned char TFT_height = 160;


#define FONT_WIDTH          5
#define FONT_HEIGHT         7
#define MAX_SPI_MSG_SIZE    4096

#define SWAP(a, b) { unsigned int t = a; a = b; b = t; }

/** Determine 5-6-5-Color value out of single RGB values */
#define Color565(r,g,b) (unsigned int)((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3)
/** Predefined color */
#define TFT_BLACK  (Color565(0  ,0  ,0  ))
/** Predefined color */
#define TFT_GRAY   (Color565(128,128,128))
/** Predefined color */
#define TFT_WHITE  (Color565(255,255,255))
/** Predefined color */
#define TFT_RED    (Color565(255,0  ,0  ))
/** Predefined color */
#define TFT_GREEN  (Color565(0  ,255,0  ))
/** Predefined color */
#define TFT_BLUE   (Color565(0  ,0  ,255))
/** Predefined color */
#define TFT_YELLOW (Color565(255,255,0  ))
/** Predefined color */
#define TFT_ORANGE (Color565(255,165,0  ))
/** Predefined color */
#define TFT_PURPLE (Color565(128,0  ,128))

/** enum for rotation*/
typedef enum
{
    DEGREE_0 = 0,  /**< don't rotate */
    DEGREE_90,     /**< rotate 90  degree CCW*/
    DEGREE_180,    /**< rotate 180 degree CCW*/
    DEGREE_270     /**< rotate 270 degree CCW*/
}rotate_E;

/* Registersettings */
#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36


#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

/**
 * TFT_ST7735 class.
 *
 * Basic functions to initialize the ST7735 Controller and draw text, lines and circles on
 * the display.
 */
class TFT_ST7735
{
  private:
    unsigned char _channel;
    unsigned char _rs;
    unsigned char _rst;
    int           _speed;
    unsigned int  _background;
    unsigned char _cursor_x;
    unsigned char _cursor_y;

    void writecommand (unsigned char);
    void writedata    (unsigned char *data, int len);
    void writedata    (unsigned char data);

    void setAddrWindow (unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
    void bulkDrawColor (unsigned int color, unsigned int length);
    void bulkDrawChar  (unsigned char x, unsigned char y, char c, unsigned int color);
    void pushColor     (unsigned int color);
    void drawFastLine  (unsigned char x, unsigned char y, unsigned char length, unsigned int color, unsigned char rotflag);

  public:
    /** @brief Constructor of the class
     *
     *  @param channel SPI channel (0 or 1)
     *  @param rs Pin number of the command/data signal
     *  @param rst Pin number of the reset signal
     *  @param speed clock rate for the SPI channel (e.g.32000000 for 32MHz)
     *
     *  @return should not return a value
     *
     */
    TFT_ST7735(unsigned char channel, unsigned char rs, unsigned char rst, int speed);

    /** @brief Common initilisation
     *
     *  Initialize SPI (via wiringPi) and reset display
     *
     *  @return void
     */
    void commonInit(void);

    /** @brief Initilisation display
     *
     *  @return void
     */
    void initB(void);

    /** @brief Initilisation display
     *
     *  @return void
     */
    void initR(void);

    /** @brief Draw a single pixel
     *
     *  @param x x-coordinate of the pixel
     *  @param y y-coordinate of the pixel
     *  @param color color of the pixel in 5-6-5 format ( see Color565(r,g,b) )

     *  @return void
     */
    void drawPixel(unsigned char x, unsigned char y, unsigned int color);

    /** @brief Clear screen with the specified color
     *
     *  @param color color of the background in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void clearScreen(unsigned int color);

    /** @brief Clear screen with the actual background color
     *
     *  @return void
     */
    void clearScreen(void);

    /** @brief Set background color to the specified color
     *
     *  @param color color of the background in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void setBackground (unsigned int color);

    /** @brief Set cursor to the specified position
     *
     *  @param x x-coordinate of the cursor
     *  @param y y-coordinate of the cursor
     *
     *  @return void
     */
    void setCursor (unsigned char x, unsigned char y);

    /** @brief Draw a string using the cursor position
     *
     *  @param *c pointer to the string
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *  @param size  factor for the size (1 = standard)
     *
     *  @return void
     */
    void drawString(const char *c, unsigned int color, unsigned char size);

    /** @brief Draw a string using the specified position
     *
     *  @param x start position of the string
     *  @param y start position of the string
     *  @param *c pointer to the string
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *  @param size  factor for the size (1 = standard)
     *
     *  @return void
     */
    void drawString(unsigned char x, unsigned char y, const char *c, unsigned int color, unsigned char size);

    /** @brief Draw a single character using the cursor position
     *
     *  @param c character to display
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *  @param size  factor for the size (1 = standard)
     *
     *  @return void
     */
    void drawChar(char c, unsigned int color, unsigned char size);

    /** @brief Draw a single character using the specified position
     *
     *  @param x start position of the character
     *  @param y start position of the character
     *  @param c character to display
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *  @param size  factor for the size (1 = standard)
     *
     *  @return void
     */
    void drawChar(unsigned char x, unsigned char y, char c, unsigned int color, unsigned char size);

    /** @brief Draw a vertical line
     *
     *  @param x start position of the line
     *  @param y start position of the line
     *  @param length length of the line
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);

    /** @brief Draw a horizontal line
     *
     *  @param x start position of the line
     *  @param y start position of the line
     *  @param length length of the line
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);

    /** @brief Draw a user defined line
     *
     *  @param x0 start position of the line
     *  @param y0 start position of the line
     *  @param x1 end position of the line
     *  @param y1 end position of the line
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void drawLine(unsigned int x0, unsigned int y0, int x1, int y1, unsigned int color);

    /** @brief Draw a rectangle
     *
     *  @param x upper left corner of the rectangle
     *  @param y upper left corner of the rectangle
     *  @param w width of the rectangle
     *  @param h height of the rectangle
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);

    /** @brief Draw a filled rectangle
         *
         *  @param x upper left corner of the rectangle
         *  @param y upper left corner of the rectangle
         *  @param w width of the rectangle
         *  @param h height of the rectangle
         *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
         *
         *  @return void
         */
    void fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);

    /** @brief Draw a circle
     *
     *  @param x0 center of the circle
     *  @param y0 center of the circle
     *  @param r radius of the circle
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color);

    /** @brief Draw a filled circle
     *
     *  @param x0 center of the circle
     *  @param y0 center of the circle
     *  @param r radius of the circle
     *  @param color color in 5-6-5 format ( see Color565(r,g,b) )
     *
     *  @return void
     */
    void fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color);

    /** @brief rotate display
     *
     *  @param value true: rotate display by 180 degree
     *
     *  @return void
     */

    void setRotation(bool value);

    /** @brief rotate display
     *
     *  @param value enum rotate_E
     *
     *  @return void
     */
    void setRotation(rotate_E value);



};

#endif // TFT1_8_H
