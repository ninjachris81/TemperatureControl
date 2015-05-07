#ifndef settings_h
#define settings_h

#include <EEPROM.h>

#include "temperature_logic.h"
#include "input_handler.h"

#define CONFIG_VERSION 1
#define CONFIG_START 32
#define CONFIG_EMPTY 255

#define SETTINGS_MODULE_NAME F("SETT")

class Settings : public InputHandler::InputListener {
  public:

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
/*  
    struct SettingsStruct {
      char configVersion;
      int tempHC_operatingTemp;
      int tempW_operatingTemp;
      int tempSwitches1;
    } settingsData = {
      CONFIG_VERSION,
      50,
      35,
      0
    };
    */
    
    struct SettingsStruct {
      unsigned char configVersion;
      TemperatureLogic::TempSettingsStruct temp;
    } settingsData;
    
    void init();

    void loadSettings();
    void saveSettings();

private:
    void defaultSettings();
    
};

#endif

