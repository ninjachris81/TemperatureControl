#ifndef globals_h
#define globals_h

//#include "log_handler.h"

#define ENABLED_STRING F("enabled")
#define DISABLED_STRING F("disabled")

#define RAM_UPDATE_INTERVAL_MS 10000
static unsigned long lastRamUpdate = 0;
static int freeRam = 0;

#endif
