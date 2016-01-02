#ifndef remotectrl_logic_h
#define remotectrl_logic_h

#include "globals.h"
#include "iocontroller.h"

#include "VirtualWire.h"

#define REMOTECTRL_MODULE_NAME MODNAME("RC")

#define CMD_SWITCH_OFF 0
#define CMD_SWITCH_ON 1

#define ON_PING_TIMEOUT_MS 10000

class RemoteCtrlLogic {
public:
  RemoteCtrlLogic();
  ~RemoteCtrlLogic();

  void init(IOController *ioController);
  void update();
  
private:
  IOController *ioController;

  void checkTimeout();

  unsigned long lastOnPing = 0;
};

#endif

