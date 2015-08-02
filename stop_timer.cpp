#include "stop_timer.h"

void StopTimer::start() {
  if (this->running) return;
  
  this->running = true;
  this->startTimeMs = millis();
}

void StopTimer::stop() {
  if (!this->running) return;
  
  this->running = false;
  this->durationMs+=millis()-this->startTimeMs;
  this->startTimeMs = 0;
}

void StopTimer::reset(bool force) {
  if (force) this->running = false;
  if (!this->running) this->startTimeMs = 0;
  this->durationMs = 0;
}

unsigned long StopTimer::currentMs() {
  if (this->running) {
    return millis()-this->startTimeMs + this->durationMs;
  } else {
    return this->durationMs;
  }
}

