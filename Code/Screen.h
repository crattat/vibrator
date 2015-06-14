#ifndef Screen_h
#define Screen_h

#include <UTFT.h>
#include <ITDB02_Touch.h>

// =========================================================================
// Screen settings
// =========================================================================

extern uint8_t BigFont[];

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define SCREEN_MAX_X 319
#define SCREEN_MAX_Y 239

#define FONT_WIDTH 16
#define FONT_HEIGHT 16
#define HEADER_HEIGHT 28

#define SCREEN_CONTRAST 100

// =========================================================================
// Just a container for the touch coordinates
// =========================================================================
typedef struct
{
  uint16_t x;
  uint16_t y;
} TouchPosition;


// =========================================================================
// The screen class encapsulates the native display driver.
// This should make it easier to align the application to other display
// devices using different libraries.
// =========================================================================
class Screen
{
  public:
    Screen();
    
    void initialize();

    void drawHeader(char* title);

    void clear();
    void setBackColor(uint8_t r, uint8_t g, uint8_t b);
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    void drawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void drawPixel(uint16_t x, uint16_t y);
    void drawText(uint16_t x, uint16_t y, char* text);
    void drawInteger(uint16_t x, uint16_t y, int number);
    void drawInteger(uint16_t x, uint16_t y, uint16_t widht, int number);

    void fillRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    
    bool touched();
    TouchPosition touchCoordinates();
    String touchCoordinatesString();

    UTFT* screenDevice();
    ITDB02_Touch* touchDevice();

  private:
    UTFT _screen;
    ITDB02_Touch _touch;

};

#endif


