#include "wifi_logic.h"
#include "time_logic.h"
#include "log_handler.h"

void WifiLogic::init(WifiSettingsStruct *settings, TemperatureLogic *temperatureLogic, IOController *ioController) {
  this->temperatureLogic = temperatureLogic;
  this->ioController = ioController;

  this->settingsData = settings;
  
  LogHandler::logMsg(WIFI_HANDLER_MODULE_NAME, F("AP: "), this->settingsData->apIndex);
  
  this->ioController->addPropertyValueListener(PIN_PUMP_HC_INDEX, this);
  this->ioController->addPropertyValueListener(PIN_PUMP_WATER_INDEX, this);
  this->ioController->addPropertyValueListener(PIN_FLOW_SWITCH_INDEX, this);
  
  InputHandler::registerListener(this);

  myEsp.init();

  myEsp.softReset();

  myEsp.echoOff();

  myEsp.test();
  
  if (!connectAp()) return;

  myClient.init(&myEsp);
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
  } else if (cmd.startsWith(F("SYNC"))) {
    syncData(0);
  }
}


void WifiLogic::onPropertyValueChange(uint8_t id, int value) {
  switch(id) {
    case PIN_PUMP_HC_INDEX:
      setStopTimer(hcST, value);
      break;
    case PIN_PUMP_WATER_INDEX:
      setStopTimer(wST, value);
      break;
    case PIN_FLOW_SWITCH_INDEX:
      setStopTimer(fsST, value);
      break;   
  }
}

void WifiLogic::setStopTimer(StopTimer &timer, int value) {
    if (value==IO_STATE_ON) {
      timer.start(); 
    } else { 
      timer.stop();
    }
}

bool WifiLogic::connectAp() {
  String ssid;
  String pw;

  switch(this->settingsData->apIndex) {
    case WIFI_AP_STALNET_REPEAT:
      ssid = F("StalnetRepeat");
      pw = F("wlandome");
      break;
    case WIFI_AP_STALNET:
      ssid = F("Stalnet");
      pw = F("wlandome");
      break;
    default:
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Unknown AP - using default"));
      ssid = F("StalnetRepeat");
      pw = F("wlandome");
      break;
  }
  
  bool returnVal = myEsp.apJoin(ssid, pw);
  if (returnVal) {
    status = WIFI_STATUS_CONNECTED_AP;
  }
  return returnVal;
}


void WifiLogic::update(int freeRam) {
  if (lastUpdate==0 || millis() - lastUpdate >= WIFI_CHECK_INTERVAL_MIN_MS) {
    lastUpdate = millis();
    
    if (status!=WIFI_STATUS_CONNECTED_AP){
      if (!connectAp()) return;
    }
    
    syncData(freeRam);
  }
}

void WifiLogic::syncData(int freeRam) {
  if (myClient.connectToServer(WIFI_REMOTE_IP)) {
    String response = "";
    String query = F("/update?key=CZPKJGC87RGLCY6J");
    
    // tempW
    addParam(query, 1, temperatureLogic->getCurrentTemperatureW());
    
    // tempHC
    addParam(query, 2, temperatureLogic->getCurrentTemperatureHC());

    // tempTank
    addParam(query, 3, temperatureLogic->getCurrentTemperatureTank());

    // pumpW
    addParam(query, 4, ((int)wST.currentMs()/1000));

    // pumpHC
    addParam(query, 5, ((int)hcST.currentMs()/1000));

    // flowSwitch
    addParam(query, 6, ((int)fsST.currentMs()/1000));

    // free ram
    addParam(query, 7, freeRam);


    if (firstTime) {
      addParam(query, 8, 1);
      firstTime = false;
    }

    if (!myClient.executeGET(query, WIFI_REMOTE_HOST, HTTP_CONN_MODE_CLOSE, response)) {
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Error @ GET"));
    } else {
      uint8_t h, m;
      parseDate(response, h, m);

      // temp hack
      h+=2;
      h=h%24;

      
      TimeLogic::save(h, m, 0);

      wST.reset();
      hcST.reset();
      fsST.reset();
    }
  } else {
    LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Connect Fail"));
  }

  myClient.disconnectFromServer();
}

void WifiLogic::addParam(String &query, uint8_t index, int value) {
  query+=F("&field");
  query+=index;
  query+=F("=");
  query+=value;
}


void WifiLogic::parseDate(String inputStr, uint8_t &h, uint8_t &m) {
  int i = inputStr.indexOf(F("Date:"));
  if (i!=-1) {
    String date = inputStr.substring(i+6, i+6+35);
    int o = date.indexOf(F("\r\n"));
    date = date.substring(0, o);
    int s, d, Y;
    char M[4];

    char dateArr[date.length()];
    date.toCharArray(dateArr, date.length());
    
    sscanf(dateArr, "%*[a-zA-Z,] %d %s %d %d:%d%:%d", &d, M, &Y, &h, &m, &s);
  }
}

void WifiLogic::reportError(String errorMsg) {
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
}
