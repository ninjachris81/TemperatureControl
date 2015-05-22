#include "serial_api.h"

void SerialApi::init() {
  Serial.begin(9600);
}

void SerialApi::update() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c!=';') {
      tmpBuffer.concat(c);
    } else {
      InputHandler::executeCmd(tmpBuffer);
      tmpBuffer = "";
    }
  }
}


