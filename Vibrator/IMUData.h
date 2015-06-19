#ifndef IMUData_h
#define IMUData_h

// =========================================================================
// This class is just a container for all the sensor data to pass it around.
// It provides some helper functions to convert the sensor data into
// understandable units.
// =========================================================================
class IMUData
{
  public:
    IMUData()
    {
      ax = ay = az = gx = gy = gz = t = r = 0;
    }

    //=========================================================
    // Raw values from the IMU and other measures.
    //=========================================================
    // accelerometer
    int16_t  ax;
    int16_t  ay;
    int16_t  az;

    // gyroscope
    int16_t  gx;
    int16_t  gy;
    int16_t  gz;

    // temperature
    int16_t  t;

    // revolution in 60°/s
    int32_t  r;

    //=========================================================
    // Gets the temperature in degree celsius. The datasheet
    // for the MPU6050 defines a resolution of 340 per degree
    // celsius and a value of -521 at 35 degrees. 
    // To convert the measured value to °C use this formula:
    // °C = (measured value + 521 + 340 * 35) / 340
    //=========================================================
    int16_t TemperatureC()
    {
      return (t + 521 + 340 * 35) / 340;
    }

    //=========================================================
    // Gets the revolution per second.
    //=========================================================
    int16_t RevolutionPerSecond()
    {
      return r / 6;
    }

    //=========================================================
    // Gets the revolution per minute.
    //=========================================================
    int16_t RevolutionPerMinute()
    {
      return r * 10;
    }

    //=========================================================
    // Gets the accelerator X value in milli G
    //=========================================================
    int16_t AccXMilliG()
    {
      return ax * 1000.0f * 2.0f / 32768.0f;
    }

    //=========================================================
    // Gets the accelerator y value in milli G
    //=========================================================
    int16_t AccYMilliG()
    {
      return ay * 1000.0f * 2.0f / 32768.0f;
    }

    //=========================================================
    // Gets the accelerator z value in milli G
    //=========================================================
    int16_t AccZMilliG()
    {
      return az * 1000.0f * 2.0f / 32768.0f;
    }

    //=========================================================
    // Gets the gyroscope X value in degree per second scaled 
    // by 10.
    //=========================================================
    int16_t GyrXDegSec()
    {
      return gx * 10.0f * 250.0f / 32768.0f;
    }

    //=========================================================
    // Gets the gyroscope Z value in degree per second scaled 
    // by 10.
    //=========================================================
    int16_t GyrYDegSec()
    {
      return gy * 10.0f * 250.0f / 32768.0f;
    }

    //=========================================================
    // Gets the gyroscope Z value in degree per second scaled 
    // by 10.
    //=========================================================
    int16_t GyrZDegSec()
    {
      return gz * 10.0f * 250.0f / 32768.0f;
    }

};

#endif

