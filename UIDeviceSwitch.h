// User Interface Device Switch Class
// Handles single throw switches.
// Greg de Valois 2016
// www.devicenut.com
// GPL Licence

#ifndef UIDEVICE_SWITCH_H
#define UIDEVICE_SWITCH_H

class UIDeviceSwitch
{
public:

  // initialize processor pin and default settings
  UIDeviceSwitch(uint8_t pin)
                : msecsStable(25)     // default time for debouncing
                , _pin(pin)           // remember which pin
                , _islow(0)           // set initial state
                , _lastime(0)         // initialize starting time
  {
    Platform_InitPin(_pin);
    newValue = Platform_IsLow(_pin);  // set initial value
  }

  // Sets the timings for the state transistion, can override:
  uint16_t msecsStable;  // how long is takes to recognize a stable state

  bool CheckForChange(void) // returns true if new value in newValue
  {
    if (_islow != Platform_IsLow(_pin))
    {
      _lastime = Platform_GetTime();
      _islow = !_islow;   // new current state
    }
    else if (_islow != newValue) // different than current state
    {
      uint16_t delta = (Platform_GetTime() - _lastime);
      if (delta > msecsStable) // now debounced
      {
        newValue = _islow; // set output value
        return true;
      }
    }

    return false; // still waiting for change/debounce
  }

  bool newValue; // read to get new value if CheckForChange() returns true
  // also can be read after constructor executed to get initial state

  // Modify or override these calls to customize for your envionment.
  #ifdef NOT_ARDUINO_COMPATIBLE
  virtual void     Platform_InitPin(uint8_t pin) {}:
  virtual bool     Platform_IsLow(uint8_t pin)   {};
  virtual uint32_t Platform_GetTime(void)        {};
  #else
  void     Platform_InitPin(uint8_t pin)  { pinMode(pin, INPUT_PULLUP); }
  bool     Platform_IsLow(uint8_t pin)    { return (digitalRead(pin) == LOW); }
  uint32_t Platform_GetTime(void)         { return millis(); }
  #endif

private:

  uint8_t _pin;       // processor pin
  bool _islow;        // true if switch state low
  uint32_t _lastime;  // time of last switch
};

#endif // UIDEVICE_SWITCH_H
