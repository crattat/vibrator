
// =========================================================================
// included libraries
// =========================================================================
#include "SPI.h"
#include <SdFat.h>
#include <UTFT.h>
#include <ITDB02_Touch.h>
#include <Servo.h>
#include <I2Cdev.h>
#include <MPU6050.h>

// =========================================================================
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE
// implementation is used in I2Cdev.h
// =========================================================================
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include <Wire.h>
#endif

#include <DueTimer.h>

// =========================================================================
// includes from sketch
// =========================================================================
#include "Defines.h"
#include "IMUData.h"
#include "Logger.h"
#include "Menu.h"
#include "Screen.h"
#include "Views.h"

// =========================================================================
// devices
// =========================================================================
Logger            _logger;
Screen            _screen;
MPU6050           _mpu6050;

// =========================================================================
// navigation and display stuff
// =========================================================================
bool              _navigationNeedsUpdate = false;
Menu*             _activeMenu = 0;
int16_t           _activeAction = 0;
uint32_t          _last100msUpdate = 0;
uint32_t          _last25msUpdate = 0;
uint32_t          _last10msUpdate = 0;

Menu              _mainMenu(&_screen, "Vibrator V1");
Menu              _sensorCheckMenu(&_screen, "Sensor Check");
Menu              _revMeterMenu(&_screen, "Rev Meter");
Menu              _revControlMenu(&_screen, "Rev Control");
Menu              _vibMeterMenu(&_screen, "Vibration Meter");
Menu              _settingsMenu(&_screen, "Settings");

Views             _views(&_screen);

// =========================================================================
// revolution measuring
// =========================================================================
volatile bool     _revlow = false;
volatile int32_t  _revpeaks = 0;
volatile int32_t  _revcount = 0;
volatile int32_t  _currentRevolution = 0;
volatile int32_t  _revolution = 0;

// =========================================================================
// IMU measure data
// =========================================================================
IMUData           _imuData;

// =========================================================================
// control motor
// =========================================================================
Servo  _esc;
volatile int      _escTargetRev = 0;
volatile int      _escCurrentLevel = 0;
volatile int      _escLastLevel = 0;

// =========================================================================
// states for automatic measuring
// =========================================================================
uint32_t         _testStart;
uint8_t          _step;

volatile  int32_t _revvalue = 0;

// =========================================================================
// 60 kHz timer to measure the revolution. To be accurate enough it is
// required to get updates every 60deg of rotation, i.e. the measuring
// unit that is used must indicate 6 times per rotation a change. In
// the original project this is done by glueing 6 markes of 3deg width
// on the engine cone.
// =========================================================================
void measureRevolutionTimer()
{
  // measure revolution
  // wait until channel 0 completed conversion
  while ((ADC->ADC_ISR & 0x80) == 0);
  // read channel 0
  _revvalue = ADC->ADC_CDR[7];
  // this is the normal analogRead which takes 40us by default, which is too slow for 60 kHz
  // int32_t _revvalue = analogRead(REV_METER_PIN);
  _revcount++;

  // here we measure low-high slopes
  if (_revvalue < 1000)
  {
    // if already low don't measure
    if (!_revlow)
    {
      _revpeaks++;
    }
    _revlow = true;
  }
  else
  {
    _revlow = false;
  }

  _currentRevolution = TIMER_REVOLUTION_FREQUENCY * _revpeaks / _revcount;

  if (_revcount > TIMER_REVOLUTION_FREQUENCY / 6)
  {
    _revolution = _currentRevolution;
    _revcount = 0;
    _revpeaks = 0;
  }
}


// =========================================================================
// 50 Hz timer to update the servo when any related mode is active.
// The current revolution is compared to the wanted target revolution.
// If current revolution is lower or higher the output PWM is increased
// or decreased correspondingly.
// =========================================================================
void controlServoTimer()
{
  switch (_activeAction)
  {
    case ACTION_ESC_CONTROL_1000U:
    case ACTION_ESC_CONTROL_2000U:
    case ACTION_ESC_CONTROL_3000U:
    case ACTION_ESC_CONTROL_4000U:
    case ACTION_ESC_CONTROL_5000U:
    case ACTION_VIB_METER_RECORD:

      uint16_t revMin = _revolution * 10;

      if (abs(revMin - _escTargetRev) < 100)
      {
        return;
      }

      if (revMin < _escTargetRev)
      {
        _escCurrentLevel++;
      }
      if (revMin > _escTargetRev)
      {
        _escCurrentLevel--;
      }

      if (_escCurrentLevel > REV_ESC_RANGE)
      {
        _escCurrentLevel = REV_ESC_RANGE;
      }
      else if (_escCurrentLevel < 0)
      {
        _escCurrentLevel = 0;
      }

      // don't set the same timing again
      if (_escLastLevel != _escCurrentLevel)
      {
        _escLastLevel = _escCurrentLevel;
        _esc.writeMicroseconds(map(_escCurrentLevel, 0, REV_ESC_RANGE, 800, 2000));
      }
      break;
  }
}

// =========================================================================
// Collect all sensor data in the imu data.
// =========================================================================
void updateSensorSamples()
{
  // collect from MPU6050 and revolution counter
  _mpu6050.getMotion6(&_imuData.ax, &_imuData.ay, &_imuData.az, &_imuData.gx, &_imuData.gy, &_imuData.gz);
  _imuData.t = _mpu6050.getTemperature();
  _imuData.r = _revolution;

  // just call logger which will log if recording is active
  _logger.writeAll(_revolution, _imuData);
}

// =========================================================================
// Initializes the servo.
// =========================================================================
void initializeServo()
{
  _esc.attach(SERVO_OUT);
  stopServo();
  Serial.println("Servo Initialized");
}

// =========================================================================
// Stops the servo.
// =========================================================================
void stopServo()
{
  _escTargetRev = 0;
  _escCurrentLevel = 0;
  _escLastLevel = 0;
  _esc.writeMicroseconds(map(0, 0, REV_ESC_RANGE, 800, 2000));
  Serial.println("Servo stopped");
}

// =========================================================================
// Initializes the MPU6050 and I2C bus.
// =========================================================================
void initializeIMU()
{
  // start I2C
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  // set I2C speed to 400khz, this might not work for all Arduino versions
  Wire.setClock(400000l);
  Serial.println("Using wire");
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
  Serial.println("Using fastwire");
#endif

  _mpu6050.initialize();

  Serial.println(_mpu6050.testConnection() ? "MPU6050 connected" : "MPU6050 failed");

  // todo calibrate IMU
}

// =========================================================================
// Resets the screen to be redrawn and changes to another action, if action
// is defined (> -1).
// =========================================================================
bool updateScreenSettings(int16_t activeAction, Menu* activeMenu, bool resetView)
{
  if (resetView)
  {
    _views.reset();
  }

  if (activeMenu)
  {
    _activeMenu = activeMenu;
  }

  if (activeAction >= 0)
  {
    _activeAction = activeAction;
  }
  _navigationNeedsUpdate = true;
}

// =========================================================================
// This function starts a specified action.
// =========================================================================
void runAction(int16_t action)
{
  Serial.println(action);

  switch (action)
  {
    // ==========================================
    // Vibration measuring
    // ==========================================
    case ACTION_VIB_METER:
      updateScreenSettings(action, &_vibMeterMenu, true);
      break;
    case ACTION_VIB_METER_CLOSE:
      _vibMeterMenu.reset();
      updateScreenSettings(0, &_mainMenu, true);
      break;
    case ACTION_VIB_METER_RECORD:
      _activeAction = action;
      stopServo();
      if (!_logger.recording())
      {
        delay(10);
        _escTargetRev = 1000;
        _testStart = millis();
        _step = 1;
        _logger.startRecording("vib_");
      }
      else
      {
        _testStart = 0;
        _step = 0;
        _logger.stopRecording();
      }
      break;
    // ==========================================
    // Sensor checks
    // ==========================================
    case ACTION_SENSOR_CHECK:
      updateScreenSettings(action, &_sensorCheckMenu, true);
      break;
    case ACTION_CLOSE_SENSOR_CHECK:
      _sensorCheckMenu.reset();
      updateScreenSettings(0, &_mainMenu, true);
      break;
    case ACTION_TOGGLE_SENSOR_CHECK:
      _views.toggleGraphFullScreen();
      updateScreenSettings(-1, 0, false);
      break;
    // ==========================================
    // Revolution measuring
    // ==========================================
    case ACTION_REV_METER:
      updateScreenSettings(action, &_revMeterMenu, true);
      break;
    case ACTION_REV_METER_RECORD:
      _logger.toggleRecording("rev_");
      break;
    case ACTION_REV_METER_CLOSE:
      _logger.stopRecording();
      _sensorCheckMenu.reset();
      updateScreenSettings(0, &_mainMenu, true);
      break;
    // ==========================================
    // ESC control
    // ==========================================
    case ACTION_ESC_CONTROL:
      updateScreenSettings(action, &_revControlMenu, true);
      break;
    case ACTION_ESC_CONTROL_STOP:
      _activeAction = ACTION_ESC_CONTROL;
      stopServo();
      break;
    case ACTION_ESC_CONTROL_CLOSE:
      stopServo();
      _sensorCheckMenu.reset();
      updateScreenSettings(0, &_mainMenu, true);
      break;
    case ACTION_ESC_CONTROL_1000U:
    case ACTION_ESC_CONTROL_2000U:
    case ACTION_ESC_CONTROL_3000U:
    case ACTION_ESC_CONTROL_4000U:
    case ACTION_ESC_CONTROL_5000U:
      _activeAction = action;
      stopServo();
      delay(100);
      switch (action)
      {
        case ACTION_ESC_CONTROL_1000U:
          _escTargetRev = 1000;
          break;
        case ACTION_ESC_CONTROL_2000U:
          _escTargetRev = 2000;
          break;
        case ACTION_ESC_CONTROL_3000U:
          _escTargetRev = 3000;
          break;
        case ACTION_ESC_CONTROL_4000U:
          _escTargetRev = 4000;
          break;
        case ACTION_ESC_CONTROL_5000U:
          _escTargetRev = 5000;
          break;
      }
      break;
    // ==========================================
    // Settings
    // ==========================================
    case ACTION_SETTINGS:
      updateScreenSettings(ACTION_SETTINGS, &_settingsMenu, true);
      break;
    case ACTION_SETTINGS_CLOSE:
      updateScreenSettings(0, &_mainMenu, true);
      break;
  }
}

// =========================================================================
// This update function is called every 25 milliseconds.
// =========================================================================
void update25()
{
  if (_navigationNeedsUpdate)
  {
    if (_activeMenu)
    {
      _activeMenu->draw();
    }
    _navigationNeedsUpdate = false;
  }

  switch (_activeAction)
  {
    case ACTION_SENSOR_CHECK:
      _views.updateSensorCheck(_imuData);
      break;
    case ACTION_VIB_METER:
      _views.updateSensorCheck(_imuData);
      break;
    case ACTION_REV_METER:
      _views.updateRevMeter(_imuData);
      break;
    case ACTION_ESC_CONTROL:
      _views.updateRevMeter(_imuData);
      break;
    case ACTION_ESC_CONTROL_1000U:
    case ACTION_ESC_CONTROL_2000U:
    case ACTION_ESC_CONTROL_3000U:
    case ACTION_ESC_CONTROL_4000U:
    case ACTION_ESC_CONTROL_5000U:
      _views.updateRevMeter(_imuData);
      break;
    case ACTION_VIB_METER_RECORD:
      _views.updateSensorCheck(_imuData);
      if (millis() - _testStart > 10000)
      {
        switch (_step)
        {
          case 1:
            _step++;
            _testStart = millis();
            _escTargetRev = 2000;
            break;
          case 2:
            _step++;
            _testStart = millis();
            _escTargetRev = 3000;
            break;
          default:
            _logger.stopRecording();
            _testStart = 0;
            _step = 0;
            _escTargetRev = 0;
            delay(100);
            _vibMeterMenu.reset();
            _vibMeterMenu.draw();
            updateScreenSettings(ACTION_VIB_METER, &_vibMeterMenu, true);
            stopServo();
            break;
        }
      }
      break;
    case ACTION_SETTINGS:
      _views.updateSettings(_logger.GetSdFat());
      break;
  }
}

// =========================================================================
// Initialization
// =========================================================================
void setup()
{
  Serial.begin(38400);
  Serial.println("Initializing");

  _screen.initialize();

  Serial.println("Display and touch initialized");

  if (!_logger.initialize(53, 10))
  {
    Serial.println("Logger failed");
  }
  else
  {
    Serial.println("Logger initialized");
  }

  _mainMenu.add(10,  38, 300,  30, ACTION_REV_METER, "Rev Meter");
  _mainMenu.add(10,  38 + 1 * 39, 300,  30, ACTION_ESC_CONTROL, "Rev Control");
  _mainMenu.add(10,  38 + 2 * 39, 300,  30, ACTION_VIB_METER, "Vibration Meter");
  _mainMenu.add(10,  38 + 3 * 39, 300,  30, ACTION_SENSOR_CHECK, "Sensor Check");
  _mainMenu.add(10,  38 + 4 * 39, 300,  30, ACTION_SETTINGS, "Settings");

  _sensorCheckMenu.add(272, 5, 20, 20, ACTION_TOGGLE_SENSOR_CHECK, BUTTON_TOGGLE);
  _sensorCheckMenu.add(296, 5, 20, 20, ACTION_CLOSE_SENSOR_CHECK, BUTTON_CLOSE);

  _revMeterMenu.add(272, 5, 20, 20, ACTION_REV_METER_RECORD, BUTTON_RECORD);
  _revMeterMenu.add(296, 5, 20, 20, ACTION_REV_METER_CLOSE, BUTTON_CLOSE);

  _revControlMenu.add(296,  5, 19, 19, ACTION_ESC_CONTROL_CLOSE, BUTTON_CLOSE);
  _revControlMenu.add(5,   40, 80, 25, ACTION_ESC_CONTROL_STOP, "STOP");
  _revControlMenu.add(95,  40, 35, 25, ACTION_ESC_CONTROL_1000U, "1k");
  _revControlMenu.add(140, 40, 35, 25, ACTION_ESC_CONTROL_2000U, "2k");
  _revControlMenu.add(185, 40, 35, 25, ACTION_ESC_CONTROL_3000U, "3k");
  _revControlMenu.add(230, 40, 35, 25, ACTION_ESC_CONTROL_4000U, "4k");
  _revControlMenu.add(275, 40, 35, 25, ACTION_ESC_CONTROL_5000U, "5k");

  _vibMeterMenu.add(272, 5, 19, 19, ACTION_VIB_METER_RECORD, BUTTON_RECORD);
  _vibMeterMenu.add(296, 5, 19, 19, ACTION_VIB_METER_CLOSE, BUTTON_CLOSE);

  _settingsMenu.add(296, 5, 19, 19, ACTION_SETTINGS_CLOSE, BUTTON_CLOSE);

  _activeMenu = &_mainMenu;
  _navigationNeedsUpdate = true;

  Serial.println("Navigation initialized");

  initializeServo();

  initializeIMU();

  // initialize the ADC to run in free running mode. This allows conversion in 1 us.
  ADC->ADC_MR |= 0x80;
  // start conversion
  ADC->ADC_CR = 2;
  // enable ADC channel 0
  ADC->ADC_CHER = 0x80;

  delay(500);

  Timer1.attachInterrupt(measureRevolutionTimer).setFrequency(TIMER_REVOLUTION_FREQUENCY).start();
  Timer6.attachInterrupt(controlServoTimer).setFrequency(TIMER_SERVO_FREQUENCY).start();
  Serial.println("Timers started");

  delay(500);

  Serial.println("Initialization done");
}

// =========================================================================
// Loop
// =========================================================================
void loop()
{
  // run updates every 100ms
  uint32_t t = millis();
  if (t - _last100msUpdate > 100)
  {
    _last100msUpdate = t;
    if (_screen.touched())
    {
      Serial.println(_screen.touchCoordinatesString());
      if (_activeMenu)
      {
        int16_t action = _activeMenu->check();
        if (action > 0)
        {
          runAction(action);
          action = 0;
        }
      }
    }
  }

  // run updates every 25ms
  t = millis();
  if (t - _last25msUpdate > 25)
  {
    _last25msUpdate = t;
    update25();
    Serial.println(_revvalue);
  }

  // run updates every 10ms
  t = millis();
  if (t - _last10msUpdate > 10)
  {
    _last10msUpdate = t;
    updateSensorSamples();
  }
}


