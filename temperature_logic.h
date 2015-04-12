#ifndef temperature_logic_h
#define temperature_logic_h

#include "globals.h"

#include "dht11.h"
#include "iocontroller.h"
#include "time_logic.h"

#define DHT11_PIN_HC 4
#define DHT11_PIN_W 2

#define MINUTE10_FACTOR 6

#define CHECK_INTERVAL_MIN_MS 1000
#define CHECK_TEMP_INTERVAL_MIN_MS 5000

#define TEMPERATURE_MODULE_NAME F("Temperature")

class TemperatureLogic {
public:
    struct TempSettingsStruct {
      int operatingTemp_HC;
      int operatingTemp_W;
      byte activeTimeStart10min;
      byte activeTimeEnd10min;
      int tempSwitches1;
    } settingsData;

  void init(TempSettingsStruct &settings, IOController *ioController);
  void update();
  
  void simulateTemperature(int currentTemperatureHC, int currentTemperatureW);
  
  int getTemperatureHC();
  int getTemperatureW();  

private:
  dht11 tempSensorHC;    // heat changer
  dht11 tempSensorW;     // water 
  
  int currentTemperatureHC = 0;
  int currentTemperatureW = 0;
  
  unsigned long lastUpdate = 0;
  unsigned long lastTempCheck = 0;
  
  IOController *ioController;

  bool _updateData(bool forceUpdate, bool &hasUpdated);
  
};

#endif

