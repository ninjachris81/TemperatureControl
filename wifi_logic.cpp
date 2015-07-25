#include "wifi_logic.h"
#include "time_logic.h"
#include "log_handler.h"

void WifiLogic::init(TemperatureLogic *temperatureLogic, IOController *ioController) {
  this->temperatureLogic = temperatureLogic;
  this->ioController = ioController;
  
  myEsp.init();

  myEsp.softReset();

  myEsp.echoOff();

  myEsp.test();
  
  if (!connectAp()) return;

  myClient.init(&myEsp);
}

bool WifiLogic::connectAp() {
  bool returnVal = myEsp.apJoin(F("StalnetRepeat"), F("wlandome"));
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

    // pumpW
    addParam(query, 3, ioController->getValue(PIN_PUMP_HC_INDEX));

    // pumpHC
    addParam(query, 4, ioController->getValue(PIN_PUMP_WATER_INDEX));

    // free ram
    addParam(query, 5, freeRam);

    if (!myClient.executeGET(query, WIFI_REMOTE_HOST, HTTP_CONN_MODE_CLOSE, response)) {
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Error @ GET"));
    } else {
      uint8_t h, m;
      parseDate(response, h, m);
      TimeLogic::save(h, m, 0);
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
