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
    ErrorHandler::warning(SETTINGS_MODULE_NAME, F("Error - Config version differs: "), myVersion);
  }
}

void Settings::saveSettings() {
  LogHandler::logMsg(SETTINGS_MODULE_NAME, F("Saving settings"));

  for (unsigned int t=0; t<sizeof(settingsData); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&settingsData + t));
}


