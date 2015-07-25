#ifndef settings_h
#define settings_h

#include <EEPROM.h>

#include "input_handler.h"
#include "temperature_logic.h"
#include "iocontroller.h"

#define CONFIG_VERSION 1
#define CONFIG_START 32
#define CONFIG_EMPTY 255

#define SETTINGS_MODULE_NAME "SETT"

class Settings : public InputHandler::InputListener {
  public:

  /*virtual*/ String getName();
  /*virtual*/ bool onInput(String cmd);
  
    struct SettingsStruct {
      unsigned char configVersion;
      TemperatureLogic::TempSettingsStruct temp;
      IOController::IOSettingsStruct io;
    } settingsData;
    
    void init();

    void loadSettings();
    void saveSettings();

private:
    void defaultSettings();
    
};

#endif

