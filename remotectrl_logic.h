#ifndef remotectrl_logic_h
#define remotectrl_logic_h

#include "globals.h"
#include "iocontroller.h"
#include "temperature_logic.h"

#include <RF24.h>
#include <RF24_config.h>

#define REMOTECTRL_MODULE_NAME MODNAME("RC")

#define CMD_NONE 0
#define CMD_SWITCH_OFF 1
#define CMD_SWITCH_ON 2

#define ON_PING_TIMEOUT_MS 10000

#define BROADCAST_INTERVAL_MS 5000

class RemoteCtrlLogic {
public:
  RemoteCtrlLogic();
  ~RemoteCtrlLogic();

  void init(IOController *ioController, TemperatureLogic *tempLogic);
  void update();
  
private:
  IOController *ioController;
  TemperatureLogic *tempLogic;

  RF24 *broadcast;    // TODO: SPI PIN TO GLOBALS

  const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };

  unsigned long lastBroadcast = 0;

  void checkTimeout();

  bool sendBroadcast();

  bool isAvailable;

  struct dataStruct {
    int tempWater;
    int tempHC;
    int tempTank;
    bool pumpWater;
    bool pumpHC;
  } broadcastData;

  struct cmdStruct {
    uint8_t cmd;
  } cmdData;

  unsigned long lastOnPing = 0;
};

#endif

