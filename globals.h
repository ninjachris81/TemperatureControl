#ifndef globals_h
#define globals_h

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define MODNAME(n) (F(n))
#else
  #define MODNAME(n) (n)
#endif

// PIN DEFINITIONS
#define BT_TX_PIN 10
#define BT_RX_PIN 11

#define PIN_RC_RX 12

#define PIN_TEMP 7

#define PIN_PUMP_HC 2
#define PIN_PUMP_WATER 4
#define PIN_FLOW_SWITCH 8

#define PIN_WATCHDOG_COMM 9

//#define esp SoftwareSerial(5, 6)
#define esp Serial1

// PIN DEFINITIONS

#define ENABLED_STRING "enabled"
#define DISABLED_STRING "disabled"

#define RAM_UPDATE_INTERVAL_MS 10000
static unsigned long lastRamUpdate = 0;
static int freeRam = 0;

#endif
