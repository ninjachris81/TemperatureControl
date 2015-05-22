#include "output_handler.h"

/*
OutputHandler::OutputListener* OutputHandler::listeners[MAX_OUTPUT_LISTENERS];
uint8_t OutputHandler::listenerCount = 0;
*/

OutputHandler::OutputSink* OutputHandler::sinks[MAX_OUTPUT_SINKS];
uint8_t OutputHandler::sinkCount = 0;

void OutputHandler::sendCmd(String sender, String cmd) {
  OutputSink *sink = NULL;
  
  for (uint8_t i=0;i<sinkCount;i++) {
    sinks[i]->sendCmd(sender, cmd);
  }
}

/*
void OutputHandler::registerListener(OutputListener* outputListener) {
  listeners[listenerCount] = outputListener;
  listenerCount++;
}
*/

void OutputHandler::registerSink(OutputSink* outputSink) {
  sinks[sinkCount] = outputSink;
  sinkCount++;
}    

