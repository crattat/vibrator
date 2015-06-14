
#include "Views.h"

Views::Views(Screen* screen)
{
  _screen = screen;
  reset();
}

// =========================================================================
// Resets the views, which will cause the next update to fully redraw the
// view.
// =========================================================================
void Views::reset()
{
  _firstUpdate = true;
  _updateCounter = 0;
  _graphx = 0;
  _fullScreenGraph = false;
}

// =========================================================================
// When the graph is drawn it can be toggled between half and full screen
// height.
// =========================================================================
void Views::toggleGraphFullScreen()
{
  _firstUpdate = true;
  _updateCounter = 0;
  _graphx = 0;
  _fullScreenGraph = !_fullScreenGraph;
}

// =========================================================================
// Updates the graph showing the IMU dat and revolution.
// =========================================================================
void Views::updateGraph(IMUData data, bool updateRev, bool updateAcc)
{
  int ys, lh, yo, v;

  if (_fullScreenGraph)
  {
    yo = 28;
    ys = yo + (240 - yo) / 2;
    lh = 240 - yo;
  }
  else
  {
    yo = 130;
    ys = yo + (240 - yo) / 2;
    lh = 240 - yo;
  }

  if (_firstUpdate)
  {
    _screen->setColor(155, 155, 155);
    _screen->fillRect(0, ys - lh / 2, SCREEN_MAX_X, ys + lh / 2);
  }

  _screen->setColor(155, 155, 155);
  _screen->drawLine(_graphx, ys - lh / 2, _graphx, ys + lh / 2);

  _screen->setColor(250, 0, 0);
  _screen->drawLine((_graphx + 1) % SCREEN_WIDTH, ys - lh / 2, (_graphx + 1) % SCREEN_WIDTH, ys + lh / 2);

  _screen->setColor(0, 0, 0);
  _screen->drawPixel(_graphx, ys);

  if (updateRev)
  {
    v = map(data.RevolutionPerMinute(), 0, 10000, 0, lh / 2);
    _screen->setColor(255, 0, 0);
    _screen->drawPixel(_graphx, ys - v);
  }

  if (updateAcc)
  {
    v = map(data.AccXMilliG(), -2000, 2000, -lh / 2, lh / 2);
    _screen->setColor(0, 255, 0);
    _screen->drawPixel(_graphx, ys - v);

    v = map(data.AccYMilliG(), -2000, 2000, -lh / 2, lh / 2);
    _screen->setColor(0, 0, 255);
    _screen->drawPixel(_graphx, ys - v);

    v = map(data.AccZMilliG(), -2000, 2000, -lh / 2, lh / 2);
    _screen->setColor(0, 255, 255);
    _screen->drawPixel(_graphx, ys - v);

    v = map(data.GyrXDegSec(), -2500, 2500, -lh / 2, lh / 2);
    _screen->setColor(255, 255, 0);
    _screen->drawPixel(_graphx, ys - v);

    v = map(data.GyrYDegSec(), -2500, 2500, -lh / 2, lh / 2);
    _screen->setColor(255, 0, 255);
    _screen->drawPixel(_graphx, ys - v);

    v = map(data.GyrZDegSec(), -2500, 2500, -lh / 2, lh / 2);
    _screen->setColor(120, 200, 255);
    _screen->drawPixel(_graphx, ys - v);
  }

  if (_graphx >= SCREEN_WIDTH)
  {
    _graphx = 0;
  }
  else
  {
    _graphx++;
  }
}

// =========================================================================
// Draws the sensor check view.
// =========================================================================
void Views::updateSensorCheck(IMUData data)
{
  _updateCounter++;

  int ys = 36;
  int lh = 20;
  ys = 32;

  // if graph is displayed full screen no other output is drawn
  if (!_fullScreenGraph)
  {
    // the first update will redraw everything
    if (_firstUpdate)
    {
      _screen->setBackColor(0, 0, 0);
      _screen->setColor(0, 0, 0);
      _screen->fillRect(0, ys, 309, ys + lh);

      _screen->setColor(255, 255, 255);

      _screen->drawRect(0, ys, 319, ys + lh);
      _screen->drawText(70, ys + 2, "Us");
      _screen->drawText(195, ys + 2, "Um");
      _screen->drawText(300, ys + 2, "C");
    }

    if (_updateCounter % 5 == 0)
    {
      _screen->setBackColor(0, 0, 0);
      _screen->setColor(0, 0, 0);
      _screen->fillRect(3, ys + 1, 70, ys + lh - 1);
      _screen->fillRect(102, ys + 1, 190, ys + lh - 1);
      _screen->fillRect(230, ys + 1, 300, ys + lh - 1);

      _screen->setColor(255, 255, 255);
      _screen->drawInteger(8, ys + 2, 60, data.RevolutionPerSecond());
      _screen->drawInteger(110, ys + 2, 80, data.RevolutionPerMinute());
      _screen->drawInteger(256, ys + 2, 40, data.TemperatureC());
    }

    ys = 52;

    if (_firstUpdate)
    {
      _screen->setBackColor(0, 0, 0);
      _screen->setColor(0, 0, 0);
      _screen->fillRect(1, ys + 1, 308, ys + 3 * lh - 1);

      _screen->setColor(255, 255, 255);
      _screen->drawRect(0, ys, 319, ys + 3 * lh);
      _screen->drawRect(0, ys, 319, ys + 1 * lh - 1);
      _screen->drawText(100, ys + 2, "X");
      _screen->drawText(200, ys + 2, "Y");
      _screen->drawText(300, ys + 2, "Z");
    }

    if (_updateCounter % 5 == 0)
    {

      _screen->setColor(0, 0, 0);
      _screen->setBackColor(0, 0, 0);
      _screen->fillRect(2, ys + 4 + lh, 317, ys + lh * 3 - 1);

      _screen->setColor(255, 255, 255);
      _screen->drawRect(117, ys, 217, ys + 3 * lh);
      _screen->drawInteger( 30, ys + 4 + lh * 1, 85, data.AccXMilliG());
      _screen->drawInteger(130, ys + 4 + lh * 1, 85, data.AccYMilliG());
      _screen->drawInteger(230, ys + 4 + lh * 1, 85, data.AccZMilliG());
      _screen->drawInteger( 30, ys + 4 + lh * 2, 85, data.GyrXDegSec());
      _screen->drawInteger(130, ys + 4 + lh * 2, 85, data.GyrYDegSec());
      _screen->drawInteger(230, ys + 4 + lh * 2, 85, data.GyrZDegSec());
    }
  }
  updateGraph(data, true, true);
  _firstUpdate = false;
}


// =========================================================================
// Draws the rev meter and rev control view.
// =========================================================================
void Views::updateRevMeter(IMUData data)
{
  _updateCounter++;

  int ys = 107;
  int lh = 20;

  if (_firstUpdate)
  {
    _screen->setBackColor(0, 0, 0);
    _screen->setColor(0, 0, 0);
    _screen->fillRect(0, ys, 309, ys + lh);
    _screen->setColor(255, 255, 255);
    _screen->drawRect(0, ys, 319, ys + lh);
    _screen->drawText(60, ys + 2, "U/sec");
    _screen->drawText(235, ys + 2, "U/min");
  }
  if (_updateCounter % 5 == 0)
  {
    _screen->setBackColor(0, 0, 0);
    _screen->setColor(0, 0, 0);
    _screen->fillRect(10, ys + 1, 59, ys + lh - 1);
    _screen->fillRect(160, ys + 1, 233, ys + lh - 1);
    _screen->setColor(255, 255, 255);
    _screen->drawInteger(10, ys + 2, data.RevolutionPerSecond());
    _screen->drawInteger(160, ys + 2, data.RevolutionPerMinute());
  }

  updateGraph(data, true, true);
  _firstUpdate = false;
}

// =========================================================================
// Draws the settings view.
// =========================================================================
void Views::updateSettings(SdFat* sd)
{
  const int vx1 = 5;
  const int vx2 = 75;
  const int vx3 = 165;
  const int vx4 = 230;

  const int vy1 = 36;
  const int vy2 = 56;
  const int vy3 = 76;

  _screen->setColor(255, 255, 255);
  _screen->drawLine(  0,  vy1 - 5, SCREEN_MAX_X,  vy1 - 5);
  _screen->drawLine(  0, vy3 + 22, SCREEN_MAX_X, vy3 + 22);
  _screen->drawLine(  0,  vy1 - 5,            0, vy3 + 22);
  _screen->drawLine(160,  vy1 - 5,          160, vy3 + 22);
  _screen->drawLine(319,  vy1 - 5, SCREEN_MAX_X, vy3 + 22);

  _screen->setBackColor(0, 0, 0);
  _screen->setColor(255, 255, 255);
  _screen->drawText(vx1, vy1, "Typ: ");
  switch (sd->card()->type())
  {
    case SD_CARD_TYPE_SD1:
      _screen->drawText(vx2, vy1, "SD1");
      break;
    case SD_CARD_TYPE_SD2:
      _screen->drawText(vx2, vy1, "SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      _screen->drawText(vx2, vy1, "SDHC");
      break;
    default:
      _screen->drawText(vx2, vy1, "?");
  }

  uint32_t volumesize;
  _screen->drawText(vx1, vy2, "FAT: ");
  _screen->drawInteger(vx2, vy2, sd->vol()->fatType());

  volumesize = sd->vol()->blocksPerCluster();
  volumesize *= sd->vol()->clusterCount();
  volumesize *= 0.000512;
  _screen->drawText(vx1, vy3, "MB : ");
  _screen->drawInteger(vx2, vy3, volumesize);

  sd->chdir();
  int numFiles = 0;
  int numDirs = 0;

  SdFile file;

  while (file.openNext(sd->vwd(), O_READ))
  {
    if (file.isDir())
    {
      numDirs++;
    }
    else
    {
      numFiles++;
    }
    file.close();
  }
  _screen->drawText(vx3, vy1, "DIR: ");
  _screen->drawInteger(vx4, vy1, numDirs);

  _screen->drawText(vx3, vy2, "DAT: ");
  _screen->drawInteger(vx4, vy2, numFiles); 
}


