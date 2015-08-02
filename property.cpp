#include "property.h"

void Property::init(uint8_t id, int initialValue) {
  this->id = id;
	this->value = initialValue;
}

int Property::getValue() {
	return this->value;
}

bool Property::setValue(int value) {
  if (this->value!=value) {
    this->value = value;
    
    for (uint8_t i=0; i<listenerCount; i++) {
      valueChangeListeners[i]->onPropertyValueChange(id, value);
    }
    return true;
  } else {
    return false;
  }
}

void Property::registerValueChangeListener(ValueChangeListener* listener) {
  if (listenerCount>=MAX_VALUE_CHANGE_LISTENERS) return;
	valueChangeListeners[listenerCount] = listener;
	listenerCount++;
}
