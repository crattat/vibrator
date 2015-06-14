
#include "Screen.h"

// =========================================================================
// Initializes the display and touch device
// =========================================================================
Screen::Screen() :
  // You might need to adjust these parameters to fit your display. See UTFT library.  
  _screen(CTE28, 25, 26, 27, 28),

  // You might need to adjust these parameters to fit your touch device. See ITDB02_Touch library. 
  // You might want to use the successor UTouch instead.
  _touch(6, 5, 32, 3, 2)
{
}

// =========================================================================
// Initializes the display and touch device
// =========================================================================
void Screen::initialize()
{
  _screen.InitLCD(LANDSCAPE);
  _screen.setFont(BigFont);
  _screen.setContrast(SCREEN_CONTRAST);
  _screen.clrScr();

  _touch.InitTouch(LANDSCAPE);
  _touch.setPrecision(PREC_MEDIUM);
}

// =========================================================================
// Clears the screen.
// =========================================================================
void Screen::clear()
{
  _screen.clrScr();
}

// =========================================================================
// Sets the back color for any fill methods.
// =========================================================================
void Screen::setBackColor(uint8_t r, uint8_t g, uint8_t b)
{
  _screen.setBackColor(r, g, b);
}

// =========================================================================
// Sets the forecolor for text and lines
// =========================================================================
void Screen::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  _screen.setColor(r, g, b);
}

// =========================================================================
// Draws a filled round rectangle. 
// =========================================================================
void Screen::fillRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  _screen.fillRoundRect(x1, y1, x2, y2);
}

// =========================================================================
// Draws a filled rectangle. 
// =========================================================================
void Screen::fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  _screen.fillRect(x1, y1, x2, y2);
}

// =========================================================================
// Draws a non filled round rectangle. 
// =========================================================================
void Screen::drawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  _screen.drawRoundRect(x1, y1, x2, y2);
}

// =========================================================================
// Draws a non filled rectangle. 
// =========================================================================
void Screen::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  _screen.drawRect(x1, y1, x2, y2);
}

// =========================================================================
// Draws a pixel.
// =========================================================================
void Screen::drawPixel(uint16_t x, uint16_t y)
{
  _screen.drawPixel(x, y);
}

// =========================================================================
// Draws a line.
// =========================================================================
void Screen::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  _screen.drawLine(x1, y1, x2, y2);
}

// =========================================================================
// Draws a text.
// =========================================================================
void Screen::drawText(uint16_t x, uint16_t y, char* text)
{
  _screen.print(text, x, y);
}

// =========================================================================
// Draws a number as text.
// =========================================================================
void Screen::drawInteger(uint16_t x, uint16_t y, int number)
{
  _screen.printNumI(number, x, y);
}

// =========================================================================
// Draws a number a text right-aligned.
// =========================================================================
void Screen::drawInteger(uint16_t x, uint16_t y, uint16_t width, int number)
{
  char buf[10];
  sprintf(buf, "%ld", number);
  int tx = x + width - strlen(buf) * FONT_WIDTH;
  _screen.printNumI(number, tx, y);
}

// =========================================================================
// Draws a screen header with the header background color and a specific
// text.
// =========================================================================
void Screen::drawHeader(char* title)
{
  int h = HEADER_HEIGHT;
  int x = SCREEN_WIDTH / 2 - strlen(title) * FONT_WIDTH / 2;
  int y = h / 2 - FONT_HEIGHT / 2;

  x = 10;
  _screen.setBackColor(0, 0, 128);
  _screen.setColor(0, 0, 128);
  _screen.fillRect(0, 0, SCREEN_MAX_X, h);

  _screen.setColor(255, 255, 255);
  _screen.print(title, x, y);
}

// =========================================================================
// Gets the native screen device.
// =========================================================================
UTFT* Screen::screenDevice()
{
  return &_screen;
}

// =========================================================================
// Returns true when the touch screen hasis touched.
// =========================================================================
bool Screen::touched()
{
  return _touch.dataAvailable();
}

// =========================================================================
// Gets the touched coordinates.
// =========================================================================
TouchPosition Screen::touchCoordinates()
{
  _touch.read();
  TouchPosition t;
  t.x = _touch.getX();
  t.y = _touch.getY();

  if (t.x < 0)
  {
    t.x = 0;
  }

  if (t.x > SCREEN_MAX_X)
  {
    t.x = SCREEN_MAX_X;
  }

  if (t.y < 0)
  {
    t.y = 0;
  }

  if (t.y > SCREEN_MAX_Y)
  {
    t.y = SCREEN_MAX_Y;
  }

  return t;
}

// =========================================================================
// Gets the touched coordinated as string for printing.
// =========================================================================
String Screen::touchCoordinatesString()
{
  TouchPosition t = touchCoordinates();
  return String("Touched at: ") + String (t.x) + String (", ") + String(t.y);
}

// =========================================================================
// Gets the native touch device.
// =========================================================================
ITDB02_Touch* Screen::touchDevice()
{
  return &_touch;
}





