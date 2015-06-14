#include "Menu.h"

Menu::Menu(Screen* screen, char* title)
{
  _screen = screen;
  _title = title;
}

// =========================================================================
// Sets the menu background color for released items.
// =========================================================================
void Menu::setBackgroundColor()
{
  _screen->setBackColor(0xff, 0x99, 0);
  _screen->setColor(0xff, 0x99, 0);
}

// =========================================================================
// Sets the menu background color for pressed items.
// =========================================================================
void Menu::setBackgroundColorPressed()
{
  _screen->setBackColor(0, 0, 200);
  _screen->setColor(0, 0, 200);
}

// =========================================================================
// Sets the menu text color
// =========================================================================
void Menu::setTextColor()
{
  _screen->setColor(255, 255, 255);
}

// =========================================================================
// Sets the border color.
// =========================================================================
void Menu::setBorderColor()
{
  _screen->setColor(200, 200, 200);
}

// =========================================================================
// Adds a new menu item.
// =========================================================================
int Menu::add(uint16_t x, uint16_t y, uint16_t width, uint16_t height, int16_t action, char *label)
{
  _items[_itemCount].action = action;
  _items[_itemCount].x  = x;
  _items[_itemCount].y  = y;
  _items[_itemCount].width  = width;
  _items[_itemCount].height = height;
  _items[_itemCount].label  = label;
  _items[_itemCount].t = 0;
  _items[_itemCount].pressed = false;

  switch (label[0])
  {
    case '\1':
      _items[_itemCount].t = 1;
      break;
    case '\2':
      _items[_itemCount].t = 2;
      break;
    case '\3':
      _items[_itemCount].t = 3;
      break;
  }
  
  _itemCount++;
}

// =========================================================================
// Draws all items of the menu.
// =========================================================================
void Menu::draw()
{
  _screen->clear();
  _screen->drawHeader(_title);

  for (int i = 0; i < _itemCount; i++)
  {
    draw(i, false);
  }
}

// =========================================================================
// Resets the items to it initialize status.
// =========================================================================
void Menu::reset()
{
  for (int i = 0; i < _itemCount; i++)
  {
    _items[i].pressed = false;
  }
}

// =========================================================================
// Draws a specific menu item pressed or released.
// =========================================================================
void Menu::draw(int id, bool pressed)
{
  int text_x, text_y, o;
  int x = _items[id].x;
  int y = _items[id].y;
  int h = _items[id].height;
  int w = _items[id].width;

  if (!pressed)
  {
    setBackgroundColor();
  }
  else
  {
    setBackgroundColorPressed();
  }
  _screen->fillRoundRect(x, y, x + w, y + h);

  setBorderColor();
  _screen->drawRoundRect(x, y, x + w, y + h);

  setTextColor();

  if (_items[id].label[0] == '\1')
  {
    o = 1;
    _screen->drawLine(x + o, y + o, x + w - 2 * o, y + h - 2 * o);
    _screen->drawLine(x + w - 2 * o, y + o, x + o, y + h - 2 * o);
  }
  else if (_items[id].label[0] == '\2')
  {
    o = 3;
    if (_items[id].pressed)
    {
      o = 7;
    }
    _screen->fillRoundRect(x + o, y + o, x + w - o, y + h - o );
  }
  else if (_items[id].label[0] == '\3')
  {
      o = 5;
    if (_items[id].pressed)
    {
      _screen->fillRect(x + o, y + o, x + w - o, y + h - o );
    }
    else
    {
      _screen->drawLine(x + o, y + o    , x + w - o, y + h / 2);
      _screen->drawLine(x + o, y + h - o, x + w - o, y + h / 2);
    }
  }
  else
  {
    text_x = w / 2 - strlen(_items[id].label) * FONT_WIDTH / 2 + x;
    text_y = h / 2 - FONT_HEIGHT / 2 + y;
    _screen->drawText(text_x, text_y, _items[id].label);
  }
}

// =========================================================================
// Checks whether a menu item is hit and returns the corresponding action
// in case. If no menu item is hit the function returns -1.
// =========================================================================
int16_t Menu::check()
{
  if (_screen->touched())
  {
    TouchPosition t = _screen->touchCoordinates();
    int	id = -1;
    int16_t action = 0;

    // find clicked menu and get related action
    for (int i = 0; i < _itemCount; i++)
    {
      if ((t.x >= _items[i].x) and (t.x <= (_items[i].x + _items[i].width)) and (t.y >= _items[i].y) and (t.y <= (_items[i].y + _items[i].height)))
      {
        id = i;
        action = _items[i].action;
        break;
      }
    }

    // draw activated
    if (id != -1)
    {
      draw(id, true);
    }

    // wait until screen isn't touched anymore
    while (_screen->touched()) {};

    // draw deactivated or manage toggle state
    if (id != -1)
    {
      if (_items[id].t == 2 || _items[id].t == 3)
      {
        _items[id].pressed = !_items[id].pressed;
      }
      draw(id, false);
    }

    return action;
  }

  return -1;
}




