
#include "Logger.h"

Logger::Logger()
{
  _recording = false;
  _driveReady = false;
}

bool Logger::initialize(int port, int pin)
{
  _driveReady = false;
  if (!_sd.begin(port, SPI_HALF_SPEED))
  {
    return false;
  }
  _driveReady = true;
  return true;
}

bool Logger::ready()
{
  return _driveReady;
}

bool Logger::createLogFile(char* prefix)
{
  char filename[100];
  int i = 0;
  bool found = false;
  String nextfile;
  while (!found)
  {
    nextfile = String(prefix) + String(i++) + String(".txt");
    nextfile.toCharArray(filename, 100);
    found = !_sd.exists(filename);
  }
  Serial.print("Open log: ");
  Serial.print(filename);
  bool result = _logFile.open(filename, O_CREAT | O_TRUNC | O_RDWR);
  Serial.print(" = ");
  Serial.println(result);
  return result;
}

void Logger::closeLogFile()
{
  _logFile.flush();
  _logFile.close();
}


bool Logger::startRecording(char* prefix)
{
  stopRecording();

  if (!createLogFile(prefix))
  {
    Serial.println("Open file failed.");
    return false;
  }

  Serial.println("Open file complete.");
  _recording = true;
  return true;
}

void Logger::stopRecording()
{
  if (_recording)
  {
    _recording = false;
    delay(100);
    Serial.println("Close measure file.");
    closeLogFile();
  }
}

void Logger::toggleRecording(char* prefix)
{
  if (!_recording)
  {
    startRecording(prefix);
  }
  else
  {
    stopRecording();
  }
}

bool Logger::recording()
{
  return _recording;
}

SdFat* Logger::GetSdFat()
{
  return &_sd;
}

void Logger::deleteAllLogFiles()
{
  /*
  File root = SD.open("/");
  while (true)
  {
    File entry =  root.openNextFile();
    if (!entry)
    {
      break;
    }

    bool isdir = entry.isDirectory();
    char* filename = entry.name();
    int len = strlen(filename);

    if (!isdir and len > 4)
    {
      entry.close();
      if (strstr(strlwr(filename + (len - 4)), ".txt"))
      {
        SD.remove(filename);
      }
    }
    else
    {
      entry.close();
    }
  }
  */
}


void Logger::writeAll(int32_t revPerSecond, IMUData data)
{
  if (_recording)
  {
    String logString;
    logString += millis() / 100;
    logString += ";";
    logString += revPerSecond;
    logString += ";";
    logString += data.ax;
    logString += ";";
    logString += data.ay;
    logString += ";";
    logString += data.az;
    logString += ";";
    logString += data.gx;
    logString += ";";
    logString += data.gy;
    logString += ";";
    logString += data.gz;
    logString += ";";
    logString += data.t;
    _logFile.println(logString);
  }
}



