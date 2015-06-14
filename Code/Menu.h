#ifndef Buttons_h
#define Buttons_h

#include "Screen.h"

// =========================================================================
// Actions for the menu items and 
// =========================================================================
#define ACTION_REV_METER 1
#define ACTION_VIB_METER 2
#define ACTION_SENSOR_CHECK 3
#define ACTION_SETTINGS 4
#define ACTION_CLOSE_SENSOR_CHECK 5
#define ACTION_TOGGLE_SENSOR_CHECK 6

#define ACTION_REV_METER_RECORD 100
#define ACTION_REV_METER_CLOSE 101

#define ACTION_ESC_CONTROL 200
#define ACTION_ESC_CONTROL_CLOSE 201
#define ACTION_ESC_CONTROL_STOP 202 
#define ACTION_ESC_CONTROL_1000U 203
#define ACTION_ESC_CONTROL_2000U 204
#define ACTION_ESC_CONTROL_3000U 205
#define ACTION_ESC_CONTROL_4000U 206
#define ACTION_ESC_CONTROL_5000U 207

#define ACTION_VIB_METER_CLOSE   150
#define ACTION_VIB_METER_RECORD  151

#define ACTION_SETTINGS_CLOSE 180

#define BUTTON_CLOSE "\1"
#define BUTTON_TOGGLE "\2"
#define BUTTON_RECORD "\3"

#define MAX_ITEMS 8

// =========================================================================
// This class implements a small menu system. 
// =========================================================================
class Menu
{
  public:
    Menu(Screen* screen, char* title);

    int	add(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int16_t action, char *label);
    void draw();
    int16_t check();
    void reset();

  protected:
    // definition of a menu item
    typedef struct
    {
      uint8_t t;
      bool pressed;
      int16_t action;
      uint16_t x;
      uint16_t y;
      uint16_t width;
      uint16_t height;
      char *label;
    } MenuItem;

    Screen* _screen;
    char* _title;
    int _itemCount = 0;
    MenuItem _items[MAX_ITEMS];

    void setBackgroundColor();
    void setBackgroundColorPressed();
    void setTextColor();
    void setBorderColor();
    void draw(int id, bool pressed);
};

#endif

