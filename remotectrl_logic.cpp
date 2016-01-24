#include "remotectrl_logic.h"
#include "output_handler.h"
#include "log_handler.h"
#include "error_handler.h"
#include "printf.h"

RemoteCtrlLogic::RemoteCtrlLogic() {
  broadcast = new RF24(49,53);
}

RemoteCtrlLogic::~RemoteCtrlLogic() {
  delete broadcast;
}

void RemoteCtrlLogic::init(IOController *ioController, TemperatureLogic *tempLogic) {
  this->ioController = ioController;
  this->tempLogic = tempLogic;

  printf_begin();

  broadcast->begin();

  //broadcast->setPALevel(RF24_PA_LOW);
  broadcast->setPALevel(RF24_PA_MAX);

  broadcast->openWritingPipe(pipes[0]);
  broadcast->openReadingPipe(1,pipes[1]);
  broadcast->enableAckPayload();
  broadcast->setRetries(0,15);

  broadcast->printDetails();

  //broadcast->startListening();
}

void RemoteCtrlLogic::update() {
  bool bcSuccess = false;
  
  if (lastBroadcast==0 || millis() - lastBroadcast >= BROADCAST_INTERVAL_MS) {
    bcSuccess = sendBroadcast();
    lastBroadcast = millis();
  } else {
    checkTimeout();
  }

  // Wait for a message
  if (bcSuccess) {
    if (broadcast->available()) {
      broadcast->read( &cmdData, sizeof(cmdData) );
      // Message with a good checksum received, dump it.
      LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Rcv: "), cmdData.cmd);
  
      switch(cmdData.cmd) {
        case CMD_SWITCH_OFF:
        LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Switching off"));
        lastOnPing = millis() + ON_PING_TIMEOUT_MS;
        break;
        case CMD_SWITCH_ON:
        if (lastOnPing==0) {
          LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Switching on"));
          ioController->setMode(PIN_PUMP_HC_INDEX, PUMP_MODE_ON);
        }
        lastOnPing = millis();
        break;
      }
    }
  }
}

bool RemoteCtrlLogic::sendBroadcast() {
  LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Sending BC"));
  
  broadcastData.tempWater = tempLogic->getCurrentTemperatureW();
  broadcastData.tempHC = tempLogic->getCurrentTemperatureHC();
  broadcastData.tempTank = tempLogic->getCurrentTemperatureTank();

  broadcastData.pumpWater = ioController->getPropertyValue(PIN_PUMP_WATER_INDEX)==IO_STATE_ON;
  broadcastData.pumpHC = ioController->getPropertyValue(PIN_PUMP_HC_INDEX)==IO_STATE_ON;

  LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Sending broadcast size "), sizeof(dataStruct));

  broadcast->stopListening();
  if (!broadcast->write( &broadcastData, sizeof(dataStruct),1 )){
    LogHandler::warning(REMOTECTRL_MODULE_NAME, F("Failed to send broadcast"));
    return false;
  } else {
    LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Sent broadcast"));
    return true;
  }
}

void RemoteCtrlLogic::checkTimeout() {
  if (lastOnPing==0) return;
  
  if (millis() - lastOnPing >= ON_PING_TIMEOUT_MS) {      // switch back to auto mode
    ioController->setMode(PIN_PUMP_HC_INDEX, PUMP_MODE_AUTO);
    lastOnPing = 0;
  }
}
