#include "esp8266.h"
#include "log_handler.h"

void ESP8266::init() {
  esp.begin(ESP_BAUD_RATE);
  esp.setTimeout(ESP_TIMEOUT_MS);
  clearTask();
}

void ESP8266::setTaskCallbackHandler(TaskCallbackHandler* tcbh) {
  this->tcbh = tcbh;
}

void ESP8266::clearTask() {
  currentTask.currentTimeout = ESP_DEFAULT_REQUEST_TIMEOUT_MS;
  currentTask.startedAt = 0;
  currentTask.isTimeout = false;
  
  currentTask.type = ESP_TASK_TYPE_NONE;
  currentTask.id = 0;
  currentTask.state = 0;
  currentTask.dataToSend = "";
  currentTask.sendOffset = 0;
}

void ESP8266::printTask() {
  Serial.println(currentTask.type);
}

void ESP8266::setBaudRate() {
  sendCommandAndWait(F("AT+CIOBAUD=115200"), F("OK"));
  delay(1000);
  esp.end();
  esp.begin(ESP_BAUD_RATE);
}

bool ESP8266::setTimeout(int timeoutMs) {
  String cmd = F("AT+CIPSTO=");
  cmd+=timeoutMs;
  return sendCommandAndWait(cmd);
}

void ESP8266::test() {
  currentTask.type = ESP_TASK_TYPE_TEST;
  
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Test"));
  sendCommand(F("AT"));    // echo off
}

void ESP8266::_test() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->testComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->testComplete(false);
  }
}

void ESP8266::echoOff() {
  currentTask.type = ESP_TASK_TYPE_ECHO_OFF;
  
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Echo off"));
  sendCommand(F("ATE0"));    // echo off
}

void ESP8266::_echoOff() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->echoOffComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->echoOffComplete(false);
  }
}


void ESP8266::softReset() {
  currentTask.type = ESP_TASK_TYPE_SOFT_RESET;

  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Soft reset"));
  sendCommand(F("AT+RST"));
}

void ESP8266::_softReset() {
  if (waitForResponse("wdt reset")) {
    delay(1000);
    clearTask();
    this->tcbh->softResetComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->softResetComplete(false);
  }
}

void ESP8266::setNetworkMode(uint8_t mode) {
  currentTask.type = ESP_TASK_TYPE_NETWORK_MODE;

  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Network mode"));
  String cmd = F("AT+CWMODE=");
  cmd+=mode;
  sendCommand(cmd);
}

void ESP8266::_setNetworkMode() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->networkModeComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->networkModeComplete(false);
  }
}

void ESP8266::apJoin(String name, String password) {
  currentTask.type = ESP_TASK_TYPE_AP_JOIN;

  String cmd = F("AT+CWJAP=\"");
  cmd+=name;
  cmd+="\",\"";
  cmd+=password;
  cmd+="\"";
  sendCommand(cmd);
}

void ESP8266::_apJoin() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->apJoinComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->apJoinComplete(false);
  }
}

void ESP8266::setConnMode(uint8_t mode) {
  currentTask.type = ESP_TASK_TYPE_CONN_MODE;

  String cmd = F("AT+CIPMUX=");
  cmd+=mode;
  sendCommand(cmd);
}

void ESP8266::_setConnMode() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->connModeComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->connModeComplete(false);
  }
}

void ESP8266::connectToServer(int id, String type, String ip, uint16_t port) {
  currentTask.type = ESP_TASK_TYPE_CONNECT_TO_SERVER;

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
  sendCommand(cmd);
}

void ESP8266::_connectToServer() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->connectToServerComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->connectToServerComplete(false);
  }
}

void ESP8266::getStatus() {
  sendCommand(F("AT+CIPSTATUS"));
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("STATE: "), readResponse());
}

bool ESP8266::serverMode(uint8_t mode, int port) {
  String cmd = F("AT+CIPSERVER=");
  cmd+=mode;
  if (port!=-1) {
    cmd+=",";
    cmd+=port;
  }
  return sendCommandAndWait(cmd);
}

bool ESP8266::sendData(int id, String data) {
  currentTask.dataToSend = data;
  currentTask.sendOffset = 0;
  currentTask.currentTimeout = ESP_DEFAULT_REQUEST_TIMEOUT_MS + (data.length() * ESP_SEND_DATA_TIMEOUT_FACTOR);
  currentTask.type = ESP_TASK_TYPE_SEND_DATA;
  currentTask.state = ESP_STATE_SEND_WAIT_FOR_READY;
  _sendDataInit(id, data.length());
}

void ESP8266::_sendDataInit(int id, int dataSize) {
  String cmd = F("AT+CIPSEND=");
  if (id>=0) {
    cmd+=id;
    cmd+=",";
  }
  cmd+=dataSize;
  sendCommand(cmd);
}

void ESP8266::_sendNextDataPacket() {
  int endOffset = min(currentTask.sendOffset + SEND_BUFFER_SIZE, currentTask.dataToSend.length());

  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Sending next packet: "), currentTask.sendOffset);
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Sending next packet: "), endOffset);
  
  sendCommand(currentTask.dataToSend.substring(currentTask.sendOffset, endOffset), false);
  currentTask.sendOffset = endOffset;
  
  if (currentTask.sendOffset>=currentTask.dataToSend.length()) {
    // end, all will be sent now
    LogHandler::logMsg(ESP8266_MODULE_NAME, F("wait for send"));
    currentTask.state = ESP_STATE_SEND_WAIT_FOR_SENT_OK;
  } else {
    // still data to send
    LogHandler::logMsg(ESP8266_MODULE_NAME, F("next packet"));
    currentTask.state = ESP_STATE_SEND_NEXT_PACKET;
  }
}

void ESP8266::_sendData() {

  switch(currentTask.state) {
    case ESP_STATE_SEND_WAIT_FOR_READY:
      if (waitForResponse(">")) {
        // send next
        _sendNextDataPacket();
      }
      break;
    case ESP_STATE_SEND_WAIT_FOR_SENT_OK:
      if (waitForResponse(F("SEND OK"))) {
        clearTask();
        this->tcbh->sendDataComplete(true);
      }      
      break;
    case ESP_STATE_SEND_NEXT_PACKET:
      _sendNextDataPacket();
      break;
  }
  
  if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->sendDataComplete(false);
  }
}

void ESP8266::apQuit() {
  currentTask.type = ESP_TASK_TYPE_AP_QUIT;
  sendCommand(F("AT+CWQAP"));
}

void ESP8266::_apQuit() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->apQuitComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->apQuitComplete(false);
  }  
}

bool ESP8266::apInRange(String name) {
  sendCommand(F("AT+CWLAP"));
}

void ESP8266::closeConn(int id) {
  currentTask.type = ESP_TASK_TYPE_CLOSE_CONN;

  String cmd = F("AT+CIPCLOSE");
  if (id>=0) {
    cmd+="=" + id;
  }
  sendCommand(cmd);
}

void ESP8266::_closeConn() {
  if (waitForResponse()) {
    clearTask();
    this->tcbh->closeConnComplete(true);
  } else if (currentTask.isTimeout) {
    clearTask();
    this->tcbh->closeConnComplete(false);
  }  
}

String ESP8266::getIp() {
  
}

void ESP8266::update() {
  if (currentTask.type!=ESP_TASK_TYPE_NONE) {
    if (currentTask.isTimeout) {
      clearTask();
    }

    if (currentTask.startedAt==0) {
      currentTask.startedAt = millis();
    } else if (millis() - currentTask.startedAt > currentTask.currentTimeout) {
      // timeout
      LogHandler::logMsg(ESP8266_MODULE_NAME, F("Command timeout"), currentTask.type);
      currentTask.isTimeout = true;
    }

    byte result;

    //LogHandler::logMsg(ESP8266_MODULE_NAME, F("Processing task: "), currentTask.type);
    //LogHandler::logMsg(ESP8266_MODULE_NAME, F("Processing task: "), currentTask.subType);

    switch(currentTask.type) {
      case ESP_TASK_TYPE_SOFT_RESET:
        _softReset();
        break;
      case ESP_TASK_TYPE_ECHO_OFF:
        _echoOff();
        break;
      case ESP_TASK_TYPE_TEST:
        _test();
        break;
      case ESP_TASK_TYPE_NETWORK_MODE:
        _setNetworkMode();
        break;
      case ESP_TASK_TYPE_AP_JOIN:
        _apJoin();
        break;
      case ESP_TASK_TYPE_CONN_MODE:
        _setConnMode();
        break;
      case ESP_TASK_TYPE_CONNECT_TO_SERVER:
        _connectToServer();
        break;
      case ESP_TASK_TYPE_SEND_DATA:
        _sendData();
        break;
      case ESP_TASK_TYPE_CLOSE_CONN:
        _closeConn();
        break;
      case ESP_TASK_TYPE_AP_QUIT:
        _apQuit();
        break;
      default:
        LogHandler::warning(ESP8266_MODULE_NAME, F("Unhandled task type: "), currentTask.type);
    }
  } else {
    //LogHandler::logMsg(ESP8266_MODULE_NAME, F("No tasks: "), taskQueue.count());
  } 
  
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
        LogHandler::warning(ESP8266_MODULE_NAME, F("IGNORING RESP: "), resp);
      }
    }
    
    delay(100);
    timeoutMs-=100;
  }
  
  if (timeoutMs>0) {
    LogHandler::logMsg(ESP8266_MODULE_NAME, F("RECV: "), expectedResult);
    return true;
  } else {
    //LogHandler::warning(ESP8266_MODULE_NAME, F("TIMEOUT WAITING FOR "), expectedResult);
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
  LogHandler::logMsg(ESP8266_MODULE_NAME, F("Sending cmd: "), cmd);
  
  if (includeCrLf) {
    esp.println(cmd);
  } else {
    esp.print(cmd);
  }
  
  //esp.flush();
}

