#ifndef ntp_logic_h
#define ntp_logic_h

#include "globals.h"

#define WIFI_HANDLER_MODULE_NAME MODNAME("NTP")

class NtpLogic {
public:

  void init();

  void update();

private:
  void utToTime(unsigned long ut, int &hours, int &minutes, int &seconds);
  
};

#endif
