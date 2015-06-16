# vibrator
The vibrator is a device for vibration analysis for brushless motors and propellers. Currently it uses a MPU6050 chip for vibration analysis and an OPB732 photoelectric sensor to measure the revolution. All together is controlled with an Arduino Due and a touch display with a SD card interface. 

# Hardware
I'm using an Arduino Due from SainSmart together with an SD-Shield and 2.8" touch display. To wire the sensors and ESC a protoshield is put between SD shield and Arduino. In the cases folder you will find 3D designs and ready to print STL files for cases for the Arduino and also for the sensors to mount it properly. 

# Functions

Currently these functions are available:

* Just measure and log revolution to a logfile to SD card
* Control a brushless motor via servo lib to run at predefined revolutions
* Automatically do a full vibration analysis at different revolutions
* Just watch all the sensor data graphically in a plot and printed
* Manage the files on SD card

## Restrictions

The MPU6050 is controlled through the I2C bus. This is quite limiting as it delivers not enough data per second. Just a few hundred measures is not enough to measure vibration frequency higher than a few hundred Hz. The results are good to compare motors or propellers reasonably anyway. For proper analysis of the concrete frequency with the strongest vibrations different sensors are required. 

## Planned Enhancements

Extensions for the vibrator will be a shunt to measure in addition the power consumption and a weighing cell to measure thrust. The results will be evaluated with statistical analysis with R later on.
 




