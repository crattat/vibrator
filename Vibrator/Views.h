#ifndef Views_h
#define Views_h

#include "Defines.h"
#include "Screen.h"
#include "IMUData.h"
#include "SdFat.h"

// =========================================================================
// This class implements the view draing methods. 
// =========================================================================
class Views
{
  public:
    Views(Screen* screen);

    void reset();
    void toggleGraphFullScreen();
  
    void updateSensorCheck(IMUData data);
    void updateRevMeter(IMUData data);
    void updateSettings(SdFat* sd);

  protected:
    Screen* _screen;
    bool _fullScreenGraph;
    bool _firstUpdate;
    uint16_t _graphx;
    uint32_t _updateCounter;

    void updateGraph(IMUData data, bool updateRev, bool updateAcc);
};

#endif

