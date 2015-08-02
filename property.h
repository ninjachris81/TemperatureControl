#ifndef PROPERTY_H
#define PROPERTY_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_VALUE_CHANGE_LISTENERS 6

class Property {
public:

	class ValueChangeListener {
	public:
		virtual void onPropertyValueChange(uint8_t id, int value) = 0;
	};

  void init(uint8_t id, int initialValue = 0);

	int getValue();
	bool setValue(int value);

	void registerValueChangeListener(ValueChangeListener* listener);
  
private:
	uint8_t id = 0;
	int value;
  
	ValueChangeListener* valueChangeListeners[MAX_VALUE_CHANGE_LISTENERS];
	uint8_t listenerCount = 0;
};

#endif
