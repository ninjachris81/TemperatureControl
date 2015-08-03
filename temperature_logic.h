#ifndef temperature_logic_h
#define temperature_logic_h

#include "globals.h"

#include "OneWire.h"
#include "DallasTemperature.h"
#include "iocontroller.h"
#include "time_logic.h"
#include "input_handler.h"
#include "property.h"

#define TEMP_INDEX_HC 0
#define TEMP_INDEX_W 1

#define MINUTE10_FACTOR 6

#define CHECK_INTERVAL_MIN_MS 1000
#define CHECK_TEMP_INTERVAL_MIN_MS 5000

#define TEMPERATURE_MODULE_NAME MODNAME("TEMP")

class TemperatureLogic : public InputHandler::InputListener, public Property::ValueChangeListener {
public:
  TemperatureLogic();
  ~TemperatureLogic();

    struct TempSettingsStruct {
      int operatingTempMin_HC;
      int operatingTempMin_W;
      byte operatingStart10Minutes;
      byte operatingEnd10Minutes;
      
      int preheatingTempMin_HC;
      int preheatingTempMin_W;
      byte preheatingStart10Minutes;
      byte preheatingDurationMinutes;
      
      int tempSwitches1;
    } settingsData;

  void init(TempSettingsStruct &settings, IOController *ioController);
  void update();
  
  /*virtual*/ void onPropertyValueChange(uint8_t id, int value);

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);

  int getCurrentTemperatureHC();
  int getCurrentTemperatureW();
  
private:
  OneWire *wire;
  DallasTemperature *tempSensors;

  Property tempHC, tempW;
  /*
  int currentTemperatureHC = 0;
  int currentTemperatureW = 0;
  */
  
  bool simulateTemp;
  
  unsigned long lastUpdate = 0;
  unsigned long lastTempCheck = 0;
  
  IOController *ioController;

  void _updateData(bool forceUpdate);
  void  _updateData(bool forceUpdate, bool &hasUpdatedW, bool &hasUpdatedHC);
  
  void checkDefault(bool &enablePumpW, bool &enablePumpHC);
  void checkPreheating(bool &enablePumpW, bool &enablePumpHC);
  
  void simulateTemperature(int temperatureW, int temperatureHC);
  
  void sendCurrentTemp();
  
};

#endif

