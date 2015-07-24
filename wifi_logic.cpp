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
    query+=F("&field1=");
    query+=temperatureLogic->getCurrentTemperatureW();
    
    // tempHC
    query+=F("&field2=");
    query+=temperatureLogic->getCurrentTemperatureHC();

    // pumpW
    query+=F("&field3=");
    query+=ioController->getValue(PIN_PUMP_HC_INDEX);

    // pumpHC
    query+=F("&field4=");
    query+=ioController->getValue(PIN_PUMP_WATER_INDEX);

    // free ram
    query+=F("&field5=");
    query+=freeRam;

    if (!myClient.executeGET(query, WIFI_REMOTE_HOST, HTTP_CONN_MODE_CLOSE, response)) {
      LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Error while GET"));
    } else {
      uint8_t h, m;
      parseDate(response, h, m);
      TimeLogic::save(h, m, 0);
    }
  } else {
    LogHandler::warning(WIFI_HANDLER_MODULE_NAME, F("Failed to connect to server !"));
  }

  myClient.disconnectFromServer();
}


void WifiLogic::parseDate(String inputStr, uint8_t &h, uint8_t &m) {
  int i = inputStr.indexOf(F("Date:"));
  if (i!=-1) {
    String date = inputStr.substring(i+6, i+6+35);
    int o = date.indexOf(F("\r\n"));
    date = date.substring(0, o);
    Serial.println(date);
    int s, d, Y;
    char M[4];

    char dateArr[date.length()];
    date.toCharArray(dateArr, date.length());
    
    sscanf(dateArr, "%*[a-zA-Z,] %d %s %d %d:%d%:%d", &d, M, &Y, &h, &m, &s);
  }
}
