#include "ntp_logic.h"
#include "time_logic.h"
#include "log_handler.h"

void NtpLogic::init() {
}

void NtpLogic::update() {
  if (Serial1.available()>0) {
    String tmp = Serial1.readStringUntil('\n');
    if (tmp.startsWith("UT=")) {
      int hours, minutes, seconds;

      tmp = tmp.substring(4);
      tmp.trim();
      unsigned long ut = tmp.toInt();
      
      utToTime(ut, hours, minutes, seconds);
      TimeLogic::save(hours, minutes, seconds);
    }
  }
}

void NtpLogic::utToTime(unsigned long ut, int &hours, int &minutes, int &seconds) {
  hours = (ut  % 86400L) / 3600;
  minutes = (ut  % 3600) / 60;
  seconds = ut % 60;
}

