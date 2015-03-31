#include "wifi_logic.h"
#include "globals.h"

void WifiLogic::init() {
  esp = new SoftwareSerial(10, 11);

  if (setupWiFi()) {
    esp->println(F("AT+CIFSR"));
    wait_for_esp_response(1000);
  } else {
    LogHandler::logMsg(WIFI_MODULE_NAME, F("Error while setting up Wifi"));
  }

}

bool WifiLogic::setupWiFi() {
  // try empty AT command
  LogHandler::logMsg(WIFI_MODULE_NAME, F("Wifi: AT"));
  esp->println(F("AT"));
  if (!wait_for_esp_response(1000)) return false;

  // set mode 1 (client)
  LogHandler::logMsg(WIFI_MODULE_NAME, F("Wifi: AT+CWMODE=1"));
  esp->println(F("AT+CWMODE=1"));
  if (!wait_for_esp_response(1000)) return false;

  // reset WiFi module
  esp->print(F("AT+RST\r\n"));
  if (!wait_for_esp_response(1500)) return false;
  delay(3000);
 
  // join AP
  esp->print(F("AT+CWJAP=\""));
  esp->print(SSID);
  esp->print(F("\",\""));
  esp->print(PASS);
  esp->println(F("\""));
  // this may take a while, so wait for 5 seconds
  if (!wait_for_esp_response(5000)) return false;
  
  esp->println(F("AT+CIPSTO=30"));  
  if (!wait_for_esp_response(1000)) return false;

  // start server
  esp->println(F("AT+CIPMUX=1"));
  if (!wait_for_esp_response(1000)) return false;
  
  esp->print(F("AT+CIPSERVER=1,")); // turn on TCP service
  esp->println(PORT);
  if (!wait_for_esp_response(1000)) return false;
  
  return true;
}

bool WifiLogic::wait_for_esp_response(int timeout) {
    return wait_for_esp_response(timeout, "OK\r\n");
}

bool WifiLogic::wait_for_esp_response(int timeout, char* term) {
  unsigned long t=millis();
  bool found=false;
  int i=0;
  int len=strlen(term);
  // wait for at most timeout milliseconds
  // or if OK\r\n is found
  while(millis()<t+timeout) {
    if(esp->available()) {
      buffer[i++]=esp->read();
      if(i>=len) {
        if(strncmp(buffer+i-len, term, len)==0) {
          found=true;
          break;
        }
      }
    }
  }
  buffer[i]=0;
  return found;
}

/*
bool read_till_eol() {
  static int i=0;
  if(esp.available()) {
    buffer[i++]=esp.read();
    if(i==BUFFER_SIZE)  i=0;
    if(i>1 && buffer[i-2]==13 && buffer[i-1]==10) {
      buffer[i]=0;
      i=0;
      return true;
    }
  }
  return false;
}
*/

void WifiLogic::update() {
  
}

