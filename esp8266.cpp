#include "esp8266.h"
#include "log_handler.h"

void ESP8266::init() {
  esp.begin(ESP_BAUD_RATE);
}

void ESP8266::setBaudRate() {
  sendCommandAndWait(F("AT+CIOBAUD=115200"), F("OK"));
  delay(1000);
  esp.end();
  esp.begin(ESP_BAUD_RATE);
}

bool ESP8266::setTimeout(int timeoutMs) {
  esp.setTimeout(timeoutMs);
  String cmd = F("AT+CIPSTO=");
  cmd+=timeoutMs;
  return sendCommandAndWait(cmd, F("OK"));
}

bool ESP8266::test() {
  return sendCommandAndWait(F("AT"), F("OK"));
}

bool ESP8266::echoOff() {
  return sendCommandAndWait(F("ATE0"), F("OK"));    // echo off
}


bool ESP8266::softReset() {
  bool returnVal = sendCommandAndWait(F("AT+RST"), F("OK"));
  delay(2000);
  return returnVal;
}

void ESP8266::getStatus() {
  sendCommand(F("AT+CIPSTATUS"));
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("STATE: "), readResponse());
}

bool ESP8266::setConnMode(int mode) {
  String cmd = F("AT+CIPMUX=");
  cmd+=mode;
  return sendCommandAndWait(cmd, F("OK"));
}

bool ESP8266::serverMode(int mode, int port) {
  String cmd = F("AT+CIPSERVER=");
  cmd+=mode;
  if (port!=-1) {
    cmd+=",";
    cmd+=port;
  }
  return sendCommandAndWait(cmd, F("OK"));
}

bool ESP8266::clientMode(int id, String type, String ip, int port) {
  String cmd = F("AT+CIPSTART=");
  if (id>=0) {
    cmd+=id + ",";
  }
  
  cmd+="\"";
  cmd+=type;
  cmd+="\",\"";
  cmd+=ip;
  cmd+="\",";
  cmd+=port;
  return sendCommandAndWait(cmd, F("OK"));
}


bool ESP8266::beginData(int id, uint16_t dataLength) {
  String cmd = F("AT+CIPSEND=");
  cmd+=id;
  cmd+=",";
  cmd+=dataLength;
  
  return sendCommandAndWait(cmd, ">");
}

bool ESP8266::postData(byte data) {
  esp.print(data);
}

bool ESP8266::sendData(int id, String data) {
  
  if (data.length()<=SEND_BUFFER_SIZE) {
    return _sendData(id, data);
  } else {
    int start = 0;
    int end = SEND_BUFFER_SIZE;
    
    do {
      if (!_sendData(id, data.substring(start, min(end, data.length())))) return false;
      if (end>data.length()) {
        return true;
      }
      start+=SEND_BUFFER_SIZE;
      end+=SEND_BUFFER_SIZE;
    } while(true);
    
  }
}

bool ESP8266::_sendData(int id, String data) {
  String cmd = F("AT+CIPSEND=");
  if (id>=0) {
    cmd+=id;
    cmd+=",";
  }
  cmd+=data.length();
  
  if (sendCommandAndWait(cmd, ">")) {
    return sendCommandAndWait(data, F("SEND OK"), 10000, false);
  } else {
    return false;
  }
}

bool ESP8266::apJoin(String name, String password) {
  sendCommandAndWait(F("AT+CWMODE=1"), F("OK"));

  String cmd = F("AT+CWJAP=\"");
  cmd+=name;
  cmd+="\",\"";
  cmd+=password;
  cmd+="\"";
  return sendCommandAndWait(cmd, F("OK"));
}

bool ESP8266::apQuit() {
  return sendCommandAndWait(F("AT+CWQAP"), F("OK"));
}

bool ESP8266::apInRange(String name) {
  sendCommand(F("AT+CWLAP"));
}

bool ESP8266::connClose(int id) {
  String cmd = F("AT+CIPCLOSE");
  if (id>=0) {
    cmd+="=" + id;
  }
  return sendCommandAndWait(cmd, F("OK"));
}

String ESP8266::getIp() {
  
}

void ESP8266::update() {
  
}

bool ESP8266::waitForResponse(String expectedResult, int timeoutMs) {
  String resp = "";
  recvBuffer = "";
  
  while(timeoutMs>0) {
    if (esp.available() > 0) {
      //resp = "";
      while(esp.available()>0) {
        
        //debug.print("AVAILABLE: ");
        //debug.println(esp.available());
        
        resp += esp.readString();
        delay(1);
      }

      //debug.println(resp);
      
      checkIPD(resp);
      if (resp.indexOf(expectedResult)>=0) {
        break;
      } else {
        LogHandler::warning(ESP8266_MODULE_NAME, F("IGNORING RESPONSE: "), resp);
      }
    }
    
    delay(100);
    timeoutMs-=100;
  }
  
  if (timeoutMs>0) {
    LogHandler::logMsg(ESP8266_MODULE_NAME, F("RECEIVED: "), expectedResult);
    return true;
  } else {
    LogHandler::warning(ESP8266_MODULE_NAME, F("TIMEOUT WAITING FOR "), expectedResult);
    return false;
  }
}

void ESP8266::checkIPD(String str) {
  int i = str.indexOf(F("+IPD,"));
  
  if (i!=-1) {
    String ipdStr = str.substring(i, i+20);   // read first 20 bytes
    
    //debug.print("CHECKING IPD: ");
    //debug.println(ipdStr);

    int o = ipdStr.indexOf(':');
    
    //debug.print("O: ");
    //debug.println(o);

    int ipdLength;

    if (ipdStr.charAt(7)==',') {    // assume id is only 0-9
      //debug.print(F("INDEX: "));
      //debug.println(ipdStr.substring(5, 6));
      int id = ipdStr.substring(5, 6).toInt();
      
      //debug.print(F("LENGTH: "));
      //debug.println(ipdStr.substring(7,o));
      ipdLength = ipdStr.substring(7,o).toInt();
    } else {
      ipdLength = ipdStr.substring(5,o).toInt();
      //debug.print(F("LENGTH: "));
      //debug.println(ipdLength);
    }

    //debug.print("IPD: ");
    //debug.println(str.substring(i+o+1,i+o+ipdLength+1));

    recvBuffer+=str.substring(i+o+1,i+o+ipdLength+1);
  }
}

void ESP8266::clearBuffer() {
  recvBuffer = "";
}

bool ESP8266::readResponse(byte &data, int timeoutMs) {
  if (esp.available()>0) {
    data = esp.read();
    return true;
  } else {
    return false;
  }
}

String ESP8266::readResponse(int timeoutMs) {
  if (recvBuffer.length()>0) {
    //debug.print(F("RECV BUFFER SIZE: "));
    //debug.println(recvBuffer.length());
  } else {
    //debug.print(F("RECV BUFFER EMPTY"));
  }
  
  while(timeoutMs>0) {
    if (esp.available()>0) {
      if (recvBuffer.length()>0) {
         //debug.print(F("APPENDING SIZE: "));
         //debug.println(esp.available());
      }
    
      recvBuffer+=esp.readString();
    }
    
    timeoutMs-=100;
    delay(100);
  }
  
  return recvBuffer;

//  debug.println(F("TIMEOUT WAITING FOR RESPONSE !"));
//  return "";
}

bool ESP8266::sendCommandAndWait(String cmd, String expectedResult, int timeoutMs, bool includeCrLf) {
  sendCommand(cmd, includeCrLf);
  return waitForResponse(expectedResult, timeoutMs);
}

void ESP8266::sendCommand(String cmd, bool includeCrLf) {
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Sending command: "), cmd);
  
  if (includeCrLf) {
    esp.println(cmd);
  } else {
    esp.print(cmd);
  }
  
  //esp.flush();
}

