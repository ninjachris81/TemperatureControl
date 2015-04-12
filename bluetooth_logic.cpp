#include "bluetooth_logic.h"
#include "globals.h"

SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);

void BluetoothLogic::init(TemperatureLogic *tempLogic, IOController* ioController, Settings *settings) {
  this->tempLogic = tempLogic;
  this->ioController = ioController;
  this->settings = settings;
  
  LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Activating BT"));
  BT.begin(BT_BAUD_RATE);
  
  LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Setting BT name: "), BT_NAME);
  BT.print(F("AT+NAME"));
  BT.print(BT_NAME);
  BT.flush();
}

void BluetoothLogic::update() {
  if (BT.available()) {
    currentMode = BT.read();
    LogHandler::logMsg(INPUT_HANDLER_MODULE_NAME, F("Switching to mode "), currentMode);
  }

  if (lastUpdate==0 || millis() - lastUpdate >= UPDATE_INTERVAL_MS) {      // last update check interval
    switch (currentMode) {
      case '1':  // sensor data
        sendData();
        break;
      case '0':  // off
        break;
    }
    lastUpdate = millis();
  }
}

void BluetoothLogic::sendData() {
  BT.print(F("TEMPHC="));
  BT.println(tempLogic->getTemperatureHC());
  BT.print(F("TEMPW="));
  BT.println(tempLogic->getTemperatureW());
  BT.print(F("IOPINS="));
  BT.println(ioController->getPinState(), HEX);
  
//  settings->settingsData->temp
}

