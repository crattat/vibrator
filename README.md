# vibrator
The vibrator is a device for vibration analysis for brushless motors and propellers. Currently it uses a MPU6050 chip for vibration analysis and an OPB732 photoelectric sensor to measure the revolution. All together is controlled with an Arduino Due and a touch display with a SD card interface. The results are evaluated with statistical analysis with R.

# Hardware
I'm using an Arduino Due together with an SD shield and 2.8" touch display. To wire the sensors and ESC a protoshield is put between SD shield and Arduino. In the cases folder you will find 3D designs and ready to print STL files for cases for the Arduino and also for the sensors to mount it properly. 

# Functions
Currently these functions are available:

* Measure and log revolution into a logfile to SD card
* Control a brushless motor via servo lib to run at predefined revolutions
* Automatically do a full vibration analysis at different revolutions
* Just watch all the sensor data graphically in a plot and as text
* Manage the files on SD card

## Restrictions
The MPU6050 is controlled through the I2C bus. This is quite limiting as it delivers not enough data per second. Just a few hundred samples is not enough to measure vibration frequency higher than a few hundred Hz. The results are good to compare motors or propellers reasonably anyway. For proper analysis of the concrete frequency with the strongest vibrations different sensors are required. I'm currently looking for better qualified or additional sensors.  

## Planned Enhancements
Extensions for the vibrator will be:

* A shunt to measure in addition the power consumption during the other tests
* A weighing cell to measure thrust during the tests. 

# Notes
To get the 2.8" SainSmart touch giving the correct coordinates the following defines have been adjusted:


```c++
#define PixSizeX	13.78

#define PixOffsX	411

#define PixSizeY	11.01

#define PixOffsY	378

```

The DueTimer lib conflicts with the Servo lib and the following define in the DueTimer.h file has been defined:

```c++
#define USING_SERVO_LIB	true
``` 
This makes several timers unavailabe.




