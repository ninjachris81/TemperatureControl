#ifndef temperature_logic_h
#define temperature_logic_h

#include "globals.h"

#include "OneWire.h"
#include "DallasTemperature.h"
#include "iocontroller.h"
#include "time_logic.h"
#include "input_handler.h"

#define PIN_TEMP 7

#define TEMP_INDEX_HC 0
#define TEMP_INDEX_W 1

#define MINUTE10_FACTOR 6

#define CHECK_INTERVAL_MIN_MS 1000
#define CHECK_TEMP_INTERVAL_MIN_MS 5000

#define TEMPERATURE_MODULE_NAME F("TEMP")

class TemperatureLogic : public InputHandler::InputListener {
public:
  TemperatureLogic();
  ~TemperatureLogic();

    struct TempSettingsStruct {
      int operatingTempMin_HC;
      int operatingTempMin_W;
      byte operatingStart10Minutes;
      byte operatingEnd10Minutes;
      
      int preheatingTempMin_HC;
      byte preheatingStart10Minutes;
      byte preheatingDurationMinutes;
      
      int tempSwitches1;
    } settingsData;

  void init(TempSettingsStruct &settings, IOController *ioController);
  void update();
  
  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
  int getTemperatureHC();
  int getTemperatureW();  

private:
  OneWire *wire;
  DallasTemperature *tempSensors;
  
  int currentTemperatureHC = 0;
  int currentTemperatureW = 0;
  
  unsigned long lastUpdate = 0;
  unsigned long lastTempCheck = 0;
  
  IOController *ioController;

  void _updateData(bool forceUpdate);
  void _updateData(bool forceUpdate, bool &hasUpdated);
  
  void checkDefault(bool &enablePumpW, bool &enablePumpHC);
  void checkPreheating(bool &enablePumpW, bool &enablePumpHC);
  
  void simulateTemperature(int currentTemperatureHC, int currentTemperatureW);
  
};

#endif

