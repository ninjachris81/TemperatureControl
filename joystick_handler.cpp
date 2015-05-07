#include "joystick_handler.h"


JoystickHandler::JoystickHandler() {
  this->feedbackHandler = NULL;
}

void JoystickHandler::setFeedbackHandler(JoystickFeedbackHandler *feedbackHandler) {
  this->feedbackHandler = feedbackHandler;
}

void JoystickHandler::init() {
  pinMode(INPUT_X, INPUT);
  pinMode(INPUT_Y, INPUT);
#ifdef USE_BUTTON
  pinMode(INPUT_BUTTON, INPUT);
#endif
}

void JoystickHandler::update() {
  short x = analogRead(INPUT_X);
  short y = analogRead(INPUT_Y);
#ifdef USE_BUTTON
  uint8_t btn = digitalRead(INPUT_BUTTON);
  
  if (btn==HIGH) {
    if (!BIT_CHECK(state,STATE_BUTTON)) {
      BIT_SET(state, STATE_BUTTON);
      if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_BUTTON, true);
    }
  } else {
    if (BIT_CHECK(state,STATE_BUTTON)) {
      if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_BUTTON, false);
      BIT_CLEAR(state,STATE_BUTTON);
    }
  }
#endif
  
  if (abs(CENTER_VALUE - x) > abs(CENTER_VALUE - y)) {// decide which axis has prio
    // handle horiz
    if ((x < CENTER_VALUE - VALUE_THRESHOLD)) {
      // left
      if (!BIT_CHECK(state,STATE_LEFT)) {
        BIT_SET(state,STATE_LEFT);
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_LEFT, true);
      }
    } else if ((x > CENTER_VALUE + VALUE_THRESHOLD)) {
      // right
      if (!BIT_CHECK(state,STATE_RIGHT)) {
        BIT_SET(state,STATE_RIGHT);
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_RIGHT, true);
      }
    } else {
      // none
      if (BIT_CHECK(state,STATE_LEFT)) {
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_LEFT, false);
        BIT_CLEAR(state,STATE_LEFT);
      }
      if (BIT_CHECK(state,STATE_RIGHT)) {
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_RIGHT, false);
        BIT_CLEAR(state,STATE_RIGHT);
      }
    }
  } else {
    // handle vert
    if ((y < CENTER_VALUE - VALUE_THRESHOLD)) {
      // down
      if (!BIT_CHECK(state,STATE_DOWN)) {
        BIT_SET(state,STATE_DOWN);
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_DOWN, true);
      }
    } else if ((y > CENTER_VALUE + VALUE_THRESHOLD)) {
      // up
      if (!BIT_CHECK(state,STATE_UP)) {
        BIT_SET(state,STATE_UP);
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_UP, true);
      }
    } else {

      // none
      if (BIT_CHECK(state,STATE_UP)) {
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_UP, false);
        BIT_CLEAR(state,STATE_UP);
      }
      if (BIT_CHECK(state,STATE_DOWN)) {
        if (feedbackHandler!=NULL) feedbackHandler->onKeyEvent(JoystickFeedbackHandler::KEY_DOWN, false);
        BIT_CLEAR(state,STATE_DOWN);
      }
    }
  }
  
//  Serial.println(state, BIN);
  
}

