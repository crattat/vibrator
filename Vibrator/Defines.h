#ifndef Defines_h
#define Defines_h

// =========================================================================
// Hardware specific settings
// =========================================================================

// The photo sensor is connected to this pin
#define REV_METER_PIN A0

// The range of ESC in microseconds is 1200, 0 means off, 1200 means 100%
#define REV_ESC_RANGE 1200

// We are using PWM 9 port
#define SERVO_OUT 9

// Timer frequency for revolution measuring
#define TIMER_REVOLUTION_FREQUENCY 60000

// Timer frequency for updating the servo
#define TIMER_SERVO_FREQUENCY 50

#endif


