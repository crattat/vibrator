#ifndef FlashDrive_h
#define FlashDrive_h

#include <SPI.h>
#include <SdFat.h>
#include "IMUData.h"

// =========================================================================
// This class implements a logger to write log files and manage creating
// new files.
// =========================================================================
class Logger
{
  public:
    Logger();

    bool initialize(int port, int pin);
    bool ready();

    bool createLogFile(char* prefix);
    void closeLogFile();
    void deleteAllLogFiles();
    void writeAll(int32_t revPerSecond, IMUData data);

    bool startRecording(char* prefix);
    void stopRecording();
    void toggleRecording(char* prefix);
  
    bool recording();

    SdFat* GetSdFat();
    
  private:
    bool _driveReady;
    bool _recording;
    SdFat _sd;
    SdFile _logFile;
};

#endif

