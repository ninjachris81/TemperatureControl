#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class StopTimer {
public:

  void start();
  void stop();
  void reset(bool force=false);
  unsigned long currentMs();

private:
  bool running = 0;
  unsigned long startTimeMs = 0;
  unsigned long durationMs = 0;
  
};
