#include "bluetooth_logic.h"
#include "globals.h"

SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);

void BluetoothLogic::init(TemperatureLogic *tempLogic, IOController* ioController, Settings *settings) {
  this->tempLogic = tempLogic;
  this->ioController = ioController;
  this->settings = settings;
  
  LogHandler::logMsg(BT_MODULE_NAME, F("Activating BT"));
  BT.begin(BT_BAUD_RATE);
  BT.setTimeout(2000);
  
  LogHandler::logMsg(BT_MODULE_NAME, F("Setting BT name: "), BT_NAME);
  BT.print(F("AT+NAME"));
  BT.print(BT_NAME);
  BT.flush();
  
  sendLogUpdates = false;
  sendDataUpdates = true;
  
  LogHandler::registerListener(this);
  InputHandler::registerListener(this);
}

String BluetoothLogic::getName() {
  return BT_MODULE_NAME;
}

bool BluetoothLogic::onInput(String cmd) {
  String v1;
  int v2;
  if (InputHandler::parseParameters2(cmd, v1, v2)) {
    if (v1.equals(F("LOG"))) {
      sendLogUpdates = (v2==1);
      LogHandler::logMsg(BT_MODULE_NAME, F("Setting log updates to: "), sendLogUpdates ? ENABLED_STRING : DISABLED_STRING);
      return true;
    } else if (v1.equals(F("DATA"))) {
      sendDataUpdates = (v2==1);
      LogHandler::logMsg(BT_MODULE_NAME, F("Setting data updates to: "), sendDataUpdates ? ENABLED_STRING : DISABLED_STRING);
      return true;
    }
  }
  return false;
}

void BluetoothLogic::update() {
  if (BT.available()) {
    String cmd = BT.readStringUntil(';');
    
  }

  if (lastUpdate==0 || millis() - lastUpdate >= UPDATE_INTERVAL_MS) {      // last update check interval
    if (sendDataUpdates) sendData();
    lastUpdate = millis();
  }
}

void BluetoothLogic::sendData() {
  BT.print(F("TEMP "));
  BT.print(tempLogic->getTemperatureHC());
  BT.print(F(" "));
  BT.print(tempLogic->getTemperatureW());
  BT.println(BT_CMD_SEP);
  
  BT.print(F("IO "));
  BT.print(ioController->getPinState(), HEX);
  BT.println(BT_CMD_SEP);

  BT.print(F("TIME "));
  BT.print((RTC.hour * 24 * 60) + (RTC.minute * 60) + RTC.second);
  BT.println(BT_CMD_SEP);
}

void BluetoothLogic::onMessage(String msg, LogHandler::LOG_TYPE type) {
  String tStr;
  
  if (sendLogUpdates) {  
    switch(type) {
      case LogHandler::LOG:
        tStr = F("LOG ");
        break;
      case LogHandler::WARN:
        tStr = F("WARN ");
        break;
      case LogHandler::FATAL:
        tStr = F("FATAL ");
        break;
      default:
        tStr = F("UNKNOWN ");
        break;
    }
    
    BT.print(tStr);
    BT.println(msg);
  }
}
