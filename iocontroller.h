#ifndef iocontroller_h
#define iocontroller_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "input_handler.h"
#include "property.h"
#include "globals.h"

#define PIN_PUMP_HC_INDEX 0
#define PIN_PUMP_WATER_INDEX 1
#define PIN_FLOW_SWITCH_INDEX 2

#define PUMP_MODE_OFF 0
#define PUMP_MODE_ON 1
#define PUMP_MODE_AUTO 2

#define IO_STATE_OFF 0
#define IO_STATE_ON 1

#define IOC_MODULE_NAME MODNAME("IOC")

class IOController : public InputHandler::InputListener, public Property::ValueChangeListener {
public:
  struct IOSettingsStruct {
    byte ioModes[3];
  } *settingsData;
    
  void init(IOSettingsStruct *settings);
  
  void update();

  void setValue(int pin, int pinIndex, bool enable);
  void setValue(int pin, int pinIndex, bool enable, bool force);

  int getPropertyValue(int pinIndex);

  void addPropertyValueListener(int pinIndex, Property::ValueChangeListener *valueChangeListener);
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
private:
  Property hcState, wState, fsState;
  
  /*virtual*/ void onPropertyValueChange(uint8_t id, int value);

  void setPropertyValue(int pinIndex, int value);

  void _printState(String ioName, bool enabled);
  
  void _sendIOState();
};


#endif

