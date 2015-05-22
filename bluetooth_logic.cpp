#include "bluetooth_logic.h"
#include "globals.h"

SoftwareSerial BT(BT_TX_PIN, BT_RX_PIN);

void BluetoothLogic::init() {
  LogHandler::logMsg(BT_MODULE_NAME, F("Activating BT"));
  BT.begin(BT_BAUD_RATE);
  BT.setTimeout(2000);
  
  LogHandler::logMsg(BT_MODULE_NAME, F("Setting BT name: "), BT_NAME);
  BT.print(F("AT+NAME"));
  BT.print(BT_NAME);
  BT.flush();
  
  sendLogUpdates = false;
  sendDataUpdates = true;
  
  isAuthenticated = false;
  
  LogHandler::registerListener(this);
  InputHandler::registerListener(this);
  OutputHandler::registerSink(this);
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
    } else if (v1.equals(F("AUTH"))) {
      isAuthenticated = (v2==BT_AUTH_HASH);
      LogHandler::logMsg(BT_MODULE_NAME, F("BT is authenticated: "), isAuthenticated ? ENABLED_STRING : DISABLED_STRING);
      return true;
    }
  }
  
  return false;
}

void BluetoothLogic::sendCmd(String sender, String cmd) {
  if (isAuthenticated) {
    LogHandler::logMsg(BT_MODULE_NAME, F("Sending cmd:"), cmd);
    BT.print(sender);
    BT.print(F(" "));
    BT.print(cmd);
    BT.print(BT_CMD_SEP);
    BT.flush();
  } else {
    LogHandler::logMsg(BT_MODULE_NAME, F("Cannot send command - not authenticated !"), cmd);
  }
}

void BluetoothLogic::update() {
  if (BT.available()) {
    String cmd = BT.readStringUntil(';');
    cmd.substring(0, cmd.length()-1);
    InputHandler::executeCmd(cmd);
  }
}

void BluetoothLogic::onLogMessage(String msg, LogHandler::LOG_TYPE type) {
  msg.replace(' ', '_');

  String cmdStr = F("LOG ");
  
  if (sendLogUpdates) {  
    switch(type) {
      case LogHandler::LOG:
        cmdStr = F("L ");
        break;
      case LogHandler::WARN:
        cmdStr = F("W ");
        break;
      case LogHandler::FATAL:
        cmdStr = F("F ");
        break;
      default:
        cmdStr = F("U ");
        break;
    }
    
    cmdStr.concat(msg);
    OutputHandler::sendCmd(BT_MODULE_NAME, cmdStr);
  }
}
