#ifndef joystick_handler_h
#define joystick_handler_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define USE_BUTTON

#define INPUT_X A0
#define INPUT_Y A1
#ifdef USE_BUTTON
  #define INPUT_BUTTON 1
#endif

#define CENTER_VALUE 518
#define VALUE_THRESHOLD 20

#define STATE_LEFT 1
#define STATE_RIGHT 2
#define STATE_UP 3
#define STATE_DOWN 4
#ifdef USE_BUTTON
  #define STATE_BUTTON 5
#endif

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

class JoystickHandler {
public:
  JoystickHandler();

  class JoystickFeedbackHandler {
    public:
    virtual void onLeft(bool isDown) = 0;
    virtual void onRight(bool isDown) = 0;
    virtual void onUp(bool isDown) = 0;
    virtual void onDown(bool isDown) = 0;
#ifdef USE_BUTTON
    virtual void onPressed(bool isDown) = 0;
#endif
  };
  
  void init();
  
  void update();
  
  void setFeedbackHandler(JoystickFeedbackHandler *feedbackHandler);

private:
  JoystickFeedbackHandler *feedbackHandler;
  uint8_t state = 0;  
};



#endif
