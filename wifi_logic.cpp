#include "wifi_logic.h"
#include "time_logic.h"
#include "log_handler.h"
#include "properties.h"

void WifiLogic::init(WifiSettingsStruct *settings, TemperatureLogic *temperatureLogic, IOController *ioController) {
  this->temperatureLogic = temperatureLogic;
  this->ioController = ioController;

  this->settingsData = settings;
  
  LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("AP: "), this->settingsData->apIndex);

  this->temperatureLogic->addPropertyValueListener(PROPERTY_TEMP_WATER, this);
  this->temperatureLogic->addPropertyValueListener(PROPERTY_TEMP_HC, this);
  this->temperatureLogic->addPropertyValueListener(PROPERTY_TEMP_TANK, this);
  
  this->ioController->addPropertyValueListener(PROPERTY_PUMP_HC, this);
  this->ioController->addPropertyValueListener(PROPERTY_PUMP_WATER, this);
  this->ioController->addPropertyValueListener(PROPERTY_FLOW_SWITCH, this);
  
  InputHandler::registerListener(this);

  ESP_SERIAL.begin(115200);
}

String WifiLogic::getName() {
  return WIFI_HANDLER_MODULE_NAME;
}

bool WifiLogic::onInput(String cmd) {
  if (cmd.startsWith(F("AP"))) {
    String v1;
    int v2;
    
    if (InputHandler::parseParameters2(cmd, v1, v2)) {
      LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("AP:"), v2);
      this->settingsData->apIndex = v2;
    }
  }
}


void WifiLogic::onPropertyValueChange(uint8_t id, int value) {
  LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("PROP CHANGE"), id, value);

  switch(id) {
    case PROPERTY_TEMP_WATER:
      updateFieldValue(FIELD_INDEX_WATER, value);
      break;
    case PROPERTY_TEMP_HC:
      updateFieldValue(FIELD_INDEX_HC, value);
      break;
    case PROPERTY_TEMP_TANK:
      updateFieldValue(FIELD_INDEX_TANK, value);
      break;
    case PROPERTY_PUMP_HC:
      setStopTimer(hcST, value);
      break;
    case PROPERTY_PUMP_WATER:
      setStopTimer(wST, value);
      break;
    case PROPERTY_FLOW_SWITCH:
      setStopTimer(fsST, value);
      break;   
  }
}

void WifiLogic::updateFieldValue(uint8_t index, int value) {
  LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("Field"), index, value);

  ESP_SERIAL.print(F("HTTP FIELD "));
  ESP_SERIAL.print(index);
  ESP_SERIAL.print(" ");
  ESP_SERIAL.println(value);
}

void WifiLogic::setActive(bool isActive) {
  LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("ACTIVE"), isActive);

  ESP_SERIAL.print(F("HTTP "));
  ESP_SERIAL.println(isActive ? F("ON") : F("OFF"));
}

void WifiLogic::setStopTimer(StopTimer &timer, int value) {
  if (value==IO_STATE_ON) {
    timer.start(); 
  } else { 
    timer.stop();
  }
}

void WifiLogic::update(int freeRam) {
  if (ESP_SERIAL.available()>0) {
    String tmp = ESP_SERIAL.readStringUntil('\n');

    if (tmp.startsWith("UT=")) {
      tmp = tmp.substring(3);
      tmp.trim();
      int tmpTs = tmp.toInt();

      if(tmpTs>0) {
        int h, m, s;

        h = ((tmpTs  % 86400L) / 3600); // the hour (86400 equals secs per day)
        m = ((tmpTs  % 3600) / 60); // the minute (3600 equals secs per minute)
        s = (tmpTs % 60); // the second

        TimeLogic::save(h, m, 0);
        LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("New Time"), h, m);
      } else {
        LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Invalid ts"), tmpTs);
      }
    } else if (tmp.equals("HTTP OK")) {
      LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("HTTP Success"));
      firstTime = false;
      wST.reset();
      hcST.reset();
      fsST.reset();
    } else {
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Ignoring "), tmp);
    }
  }

  if (lastUpdate==0 || millis() - lastUpdate >= WIFI_UPDATE_INTERVAL_MIN_MS) {
    lastUpdate = millis();

    // update other values
    updateFieldValue(FIELD_INDEX_PUMP_W, ((int)wST.currentMs()/1000));
    updateFieldValue(FIELD_INDEX_PUMP_HC, ((int)hcST.currentMs()/1000));
    updateFieldValue(FIELD_INDEX_FLOW_SWITCH, ((int)fsST.currentMs()/1000));
    updateFieldValue(FIELD_INDEX_FREE_RAM, freeRam);
    updateFieldValue(FIELD_INDEX_RESTARTED, firstTime ? 1 : 0);
  }

  if (firstTime) {
    // init values
    updateFieldValue(FIELD_INDEX_WATER, temperatureLogic->getCurrentTemperatureW());
    updateFieldValue(FIELD_INDEX_HC, temperatureLogic->getCurrentTemperatureHC());
    updateFieldValue(FIELD_INDEX_TANK, temperatureLogic->getCurrentTemperatureTank());
    setActive(true);
  }
}

void WifiLogic::reportError(String errorMsg) {
  // TODO
  /*
  if (myClient.connectToServer(WIFI_REMOTE_IP)) {
    String response = "";
    String query = F("/apps/thingtweet/1/statuses/update?api_key=AY7MVFNPXIYNPYQ4&status=");
    query+=errorMsg;
    
    if (!myClient.executeGET(query, WIFI_REMOTE_HOST, HTTP_CONN_MODE_CLOSE, response)) {
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Error @ GET"));
    }
  } else {
    LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Connect Fail"));
  }

  myClient.disconnectFromServer();
  */
}
