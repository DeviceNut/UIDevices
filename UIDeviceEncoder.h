// User Interface Device Encoder Class
// Handles rotary encoder devices.
// Greg de Valois 2021
// www.devicenut.com
// GPL Licence

// Note: not all Arduino environments support 'digitalPinToInterrupt'.

#pragma once

class UIDeviceEncoder
{
public:

  // pointer to interrupt service routine for encoder
  typedef void (*EncoderISR)(void);

  // Initialize processor pin and default settings.
  // Note that both pins must support handling interrupts, and
  // the passed ISR function must in turn call HandleInterrupt().
  UIDeviceEncoder(uint8_t pin1, uint8_t pin2, EncoderISR isr)
                : _pin1(pin1)         // remember which pin
                , _pin2(pin2)         // remember which pin
                , _old_state(0)       // set state vars
                , _new_state(0)
                , _states{0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}
  {
    Platform_InitPin(_pin1, isr);
    Platform_InitPin(_pin2, isr);
  }

  // Returns number of steps turned, and clears for next read.
  // (+ values for forward, - values for backward)
  int16_t GetChangeCount(void)
  {
    int16_t newstate = _new_state;
    _new_state = 0;
    return newstate;
  }

  void HandleInterrupt(void)
  {
    int val1 = Platform_ReadPin(_pin1);
    int val2 = Platform_ReadPin(_pin2);

    _old_state <<= 2;
    _old_state |= (val1 | (val2 << 1));
    _new_state += _states[_old_state & 0x0f];
  }

  // Modify or override these calls to customize for your envionment.
  #ifdef NOT_ARDUINO_COMPATIBLE
  virtual void Platform_InitPin(uint8_t pin) {}:
  virtual bool Platform_ReadPin(uint8_t pin) {};
  #else
  void Platform_InitPin(uint8_t pin, EncoderISR isr)
  {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), isr, CHANGE);
  }
  bool Platform_ReadPin(uint8_t pin) { return digitalRead(pin); }
  #endif

private:

  uint8_t _pin1, _pin2, _old_state;
  int16_t _new_state;
  int8_t _states[16];
};
