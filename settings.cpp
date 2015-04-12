#include "settings.h"
#include "globals.h"

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
  settingsData.temp.operatingTemp_HC = 50;
  settingsData.temp.operatingTemp_W = 35;
  settingsData.temp.activeTimeStart10min = 42;    // 7 am (7 * 6)
  settingsData.temp.activeTimeEnd10min = 120;      // 20 pm (20 * 6)
  settingsData.temp.tempSwitches1 = 0;
 
}
