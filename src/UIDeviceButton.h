// User Interface Device Button Class
// Handles momentary push button switches.
// Greg de Valois 2021
// www.devicenut.com
// GPL Licence

#pragma once

class UIDeviceButton
{
public:

  enum Retcode // all return codes defined here
  {
    Retcode_NoChange,         // no change has occurred
    Retcode_Single,           // single press/release
    Retcode_Double,           // double press/release
    Retcode_LPress,           // long press and hold
    Retcode_Repeat            // holding after LPress
  };

  // initialize processor pin and optionally override default settings
  // (note that long presses must be enabled to enable repeating)
  UIDeviceButton(uint8_t pin, bool dblclick = false, bool dolpress = false, bool dorepeat = false, bool pressval = 0)
                : msecsStable(25)     // default time for debouncing
                , msecsDouble(250)    // 1/4 second between presses
                , msecsLPress(1000)   // 1 second for long press
                , msecsRepeat(333)    // 1/3 second between repeats
                , msecsPressed(0)     // not pressed yet
                , nowPressed(false)   // not pressed yet
                , inDblClick(false)   // not pressed yet
                , _pin(pin)           // remember which pin
                , _pressval(pressval) // value of button when pressed
                , _dblclick(dblclick) // true to detect double-click
                , _dorepeat(dorepeat) // true to generate repeats
                , _dolpress(dolpress) // true to generate long press
                , _ispressed(false)   // initial state is not pressed
                , _havepress(false)   // must first wait for a press
                , _haveclick(false)   // no single click yet
                , _indouble(false)    // no double click yet
                , _didlpress(false)   // no long press yet
                , _gotmsecs(false)    // haven't set msecs yet
                , _lastime(0)         // initialize starting time
  {
    Platform_InitPin(_pin);
  }

  // Sets the timings for various state transistions, can override:

  uint16_t msecsStable;   // how long is takes to recognize a stable state
  uint16_t msecsDouble;   // max allowed time between the first button release and
                          //  the next button push to be recognized as a double-click
  uint16_t msecsLPress;   // how long the button is pushed before a long press is 
                          //  recognized (cannot be shorter than msecsStable)
  uint16_t msecsRepeat;   // time spent waiting while the button is pushed before
                          //  a repeat change is recognized (must be shorter than
                          // msecsLPress to be useful, but not more than msecsStable)

  // Poll this function frequently to detect button changes. When returns, button is NOT
  // pressed if Retcode_Single or Retcode_Double, but IS still pressed if Retcode_LPress.
  // If the button continues to be held down, and repeating is enabled, Retcode_LPress
  // will continue to be returned.

  Retcode CheckForChange(void)
  {
    return _process( Platform_GetTime(), Platform_IsPressed(_pin) );
  }

  // This value is updated if the return code is not Retcode_NoChange, and is the number
  // of msecs the button was held down before releasing (returns of Single/Double), or
  // has been held down (when returning LPress), or held down since the last time LPress
  // or Repeat was returned (when returning Repeat).
  uint16_t msecsPressed;

  // true if button currently pressed, and if pressed for a double click
  bool nowPressed, inDblClick;

  // Modify or override these calls to customize for your envionment.
  #ifdef NOT_ARDUINO_COMPATIBLE
  virtual void Platform_InitPin(uint8_t pin)    {}:
  virtual bool Platform_IsPressed(uint8_t pin)  {};
  virtual long Platform_GetTime(void)           {};
  #else
  void Platform_InitPin(uint8_t pin)    { pinMode(pin, INPUT_PULLUP); }
  bool Platform_IsPressed(uint8_t pin)  { return (digitalRead(pin) == _pressval); }
  long Platform_GetTime(void)           { return millis(); }
  #endif

private:

  // user controlled settings
  uint8_t _pin, _pressval;
  bool _dblclick, _dorepeat, _dolpress;

  // keeps track of the internal state
  bool _ispressed, _havepress, _haveclick;
  bool _indouble, _didlpress, _gotmsecs;
  long _lastime;

  Retcode _process(long curtime, bool ispressed)
  {
    Retcode rc = Retcode_NoChange;
    uint16_t delta = (curtime - _lastime);

    if (_ispressed != ispressed)
    {
      // if button has been released after a valid press
      // record the elapsed time since the press began
      if (!ispressed && _havepress && !_gotmsecs)
      {
        msecsPressed = delta;
        _gotmsecs  = true;
      }

      _lastime = curtime;     // save current time
      _ispressed = ispressed; // save new press state
      _didlpress = false;     // no long press yet
    }
    else if (_ispressed) // button is being pressed
    {
      if (delta > msecsStable) // now debounced
      {
        if (!_indouble) // no lpress/repeat if double-click
        {
          if (_dolpress && !_didlpress && (delta > msecsLPress))
            rc = Retcode_LPress;

          if (_dorepeat && _didlpress && (delta >= msecsRepeat))
            rc = Retcode_Repeat;
        }

        if ((rc == Retcode_LPress) || (rc == Retcode_Repeat))
        {
          msecsPressed = delta; // how long now held down
          _lastime = curtime;   // reset timer for next repeat
          _havepress = false;   // ignore when released
          _haveclick = false;   // reset for next time
          _indouble  = false;   // reset for next time
          _didlpress = true;    // did/will return LPress
        }
        else if (!_didlpress) // if not still in a long press
        {
          _havepress = true;    // in valid press
          _gotmsecs = false;    // rearm setting length

          if (_haveclick) _indouble = true; // is double-click
        }
      }
    }
    else if (_havepress)  // button released after having been pressed
    {
      if (delta > msecsStable) // now debounced
      {
        if (_indouble) // end of double-click sequence
        {
          _indouble = false;  // reset for next time
          rc = Retcode_Double;
        }
        // if not waiting for double-click or the
        // window has now expired: return single click
        else if (!_dblclick || (delta > msecsDouble))
          rc = Retcode_Single;

        else _haveclick = true; // remember a valid click

        if (rc != Retcode_NoChange)
        {
          _havepress = false; // wait for next press
          _haveclick = false; // reset for next time
        }
      }
    }

    nowPressed = _ispressed;
    inDblClick = _indouble;
    return rc;
  }
};
