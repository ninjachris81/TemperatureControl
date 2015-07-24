#include "settings.h"
#include "globals.h"
#include "output_handler.h"
#include "log_handler.h"

void Settings::init() {
  InputHandler::registerListener(this);
}

String Settings::getName() {
  return SETTINGS_MODULE_NAME;
}

bool Settings::onInput(String cmd) {
  if (cmd.equals(F("SAVE"))) {
    saveSettings();
    return true;
  } else if (cmd.equals(F("DEFAULT"))) {
    defaultSettings();
    return true;
  } else if (cmd.equals(F("GET"))) {
    String tmpStr = "CS ";
    
    // 0-3
    tmpStr.concat(settingsData.temp.operatingTempMin_HC);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.operatingTempMin_W);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.operatingStart10Minutes);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.operatingEnd10Minutes);
    tmpStr.concat(F("&"));
    
    // 4-8
    tmpStr.concat(settingsData.temp.preheatingTempMin_HC);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.preheatingTempMin_W);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.preheatingStart10Minutes);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.preheatingDurationMinutes);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.temp.tempSwitches1);
    
    // IO stuff
    // 9-10
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.io.ioModes[PIN_PUMP_HC_INDEX]);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.io.ioModes[PIN_PUMP_WATER_INDEX]);
    tmpStr.concat(F("&"));
    tmpStr.concat(settingsData.io.ioModes[PIN_FLOW_SWITCH_INDEX]);

    OutputHandler::sendCmd(SETTINGS_MODULE_NAME, tmpStr);
    
    LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Current settings: "), tmpStr);
    return true;
  }
  
  return false;
}

void Settings::loadSettings() {
  LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Loading settings"));

  unsigned char myVersion = EEPROM.read(CONFIG_START);

  if (myVersion==CONFIG_EMPTY) {
    LogHandler::logMsg(SETTINGS_MODULE_NAME, F("No configuration saved yet"));
  } else if (myVersion == CONFIG_VERSION) {
    LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Loading settings, version: "), myVersion);
    for (unsigned int t=0; t<sizeof(settingsData); t++)
      *((char*)&settingsData + t) = EEPROM.read(CONFIG_START + t);
  } else {
    LogHandler::warning(SETTINGS_MODULE_NAME, F("Error - Config version differs: "), myVersion);
  }
}

void Settings::saveSettings() {
  LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Saving settings, size: "), sizeof(settingsData));
  
  settingsData.configVersion = CONFIG_VERSION;

  for (unsigned int t=0; t<sizeof(settingsData); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&settingsData + t));
}

void Settings::defaultSettings() {
  LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Setting to default"));
  
  settingsData.configVersion = CONFIG_VERSION;
  
  settingsData.temp.operatingTempMin_HC = 32;
  settingsData.temp.operatingTempMin_W = 25;
  settingsData.temp.operatingStart10Minutes = 42;  // 7 am (7 * 6)
  settingsData.temp.operatingEnd10Minutes = 126;  // 21 pm (21 * 6)
  
  settingsData.temp.preheatingTempMin_HC = 38;
  settingsData.temp.preheatingTempMin_W = 32;
  settingsData.temp.preheatingStart10Minutes = 42;    // 7 am (7 * 6)
  settingsData.temp.preheatingDurationMinutes = 5;      // 5 min
  settingsData.temp.tempSwitches1 = 0;
  
  settingsData.io.ioModes[PIN_PUMP_HC_INDEX] = PUMP_MODE_AUTO;
  settingsData.io.ioModes[PIN_PUMP_WATER_INDEX] = PUMP_MODE_AUTO; 
  settingsData.io.ioModes[PIN_FLOW_SWITCH_INDEX] = PUMP_MODE_AUTO; 
}
