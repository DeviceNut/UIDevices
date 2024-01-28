// User Interface Device Analog Class
// Handles devices with analog inputs, like pots.
// Greg de Valois 2021
// www.devicenut.com
// GPL Licence

#pragma once

#if defined(ESP32)
#define MAX_ANALOG_VALUE 4095
#else
#define MAX_ANALOG_VALUE 1023
#endif

class UIDeviceAnalog
{
public:

  // initialize processor pin and optionally override default settings
  UIDeviceAnalog(uint8_t pin, long minval, long maxval, long max_read_value=MAX_ANALOG_VALUE)
                : newValue(0)         // initialize returned value
                , _pin(pin)           // remember which pin
                , _maxReadVal(max_read_value) // max value read
                , _minRetVal(minval)  // set min returned value
                , _maxRetVal(maxval)  // set max returned value
  {
    _dampValue = 0; // initialize value
  }

  bool CheckForChange(void) // returns true if there is a new value in newValue
  {
    // read value and map to range, apply dampening to smooth out jitter
    _dampValue = ((_dampValue * 7) + _mapvalue()) >> 3;
    if (_dampValue == newValue) return false;
    newValue = _dampValue;
    return true;
  }

  long newValue; // read to get new value if CheckForChange() returns true

  // Modify or override these calls to customize for your environment.
  #ifdef NOT_ARDUINO_COMPATIBLE
  virtual uint16_t Platform_ReadPin(uint8_t pin) {}:
  #else
  uint16_t Platform_ReadPin(uint8_t pin) { return analogRead(pin); }
  #endif

private:

  uint8_t _pin;               // processor pin
  long _dampValue;            // used to smooth out jitter
  const uint16_t _maxReadVal; // maximum value that can be read from pin
  const long _minRetVal;      // minimum value that can be returned
  const long _maxRetVal;      // maximum value that can be returned

  long _mapvalue() { return ((Platform_ReadPin(_pin) * (_maxRetVal - _minRetVal)) / _maxReadVal) + _minRetVal; }
};
