UIDevices Library
================================================================

This library is a set of classes for controlling various types of physical controls: buttons, pots (analog), switches, encoders.

The button class is particularly useful and powerful, allowing the caller to specify if supporting double click, long press, and repeat functionality. The timings for these can also be adjusted by overwriting global variables.

All of the code for the UIDevice classes are included in their
respective .h files, and the platform dependent functions have
been separated and virtualized to allow them to be overriden
by defining NOT_ARDUINO to 1 in the file that includes them,
allowing for use outside the standard Arduino environment.

#define NOT_ARDUINO 0 // 1 if overriding Platform_XXX calls

Calls that can be overriden:

Analog:
  virtual void Platform_ReadPin(uint8_t pin)

Button:
  virtual void Platform_InitPin(uint8_t pin)
  virtual bool Platform_IsPressed(uint8_t pin)
  virtual long Platform_GetTime(void)

Encoder:
  virtual void Platform_InitPin(uint8_t pin)
  virtual void Platform_ReadPin(uint8_t pin)

Switch:
  virtual void Platform_InitPin(uint8_t pin)
  virtual bool Platform_IsLow(uint8_t pin)
  virtual long Platform_GetTime(void)

After downloading, rename folder to 'UIDevices' and install in the Arduino Libraries folder.
