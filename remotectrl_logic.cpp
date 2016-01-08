#include "remotectrl_logic.h"
#include "output_handler.h"
#include "log_handler.h"
#include "error_handler.h"

RemoteCtrlLogic::RemoteCtrlLogic() {
}

RemoteCtrlLogic::~RemoteCtrlLogic() {
}

void RemoteCtrlLogic::init(IOController *ioController) {
  this->ioController = ioController;

  // Initialise the IO and ISR
  vw_set_rx_pin(PIN_RC_RX);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
}

void RemoteCtrlLogic::update() {
  //    LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Waiting for update, lu: "), lastUpdate);

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  // Wait for a message
  if (vw_wait_rx_max(200)) {
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      // Message with a good checksum received, dump it.
      LogHandler::logMsg(REMOTECTRL_MODULE_NAME, F("Rcv: "), buf[0]);

      switch(buf[0]) {
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
  } else {
    checkTimeout();
  }
}

void RemoteCtrlLogic::checkTimeout() {
  if (lastOnPing==0) return;
  
  if (millis() - lastOnPing >= ON_PING_TIMEOUT_MS) {      // switch back to auto mode
    ioController->setMode(PIN_PUMP_HC_INDEX, PUMP_MODE_AUTO);
    lastOnPing = 0;
  }
}
