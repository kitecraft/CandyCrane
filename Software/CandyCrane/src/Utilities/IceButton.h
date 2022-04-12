#pragma once
#include <Arduino.h>
#include "../CC_Config.h"

class IceButton
{
private:
	unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
public:
	bool Ready() {
		if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_TIME)
		{
			lastDebounceTime = millis();
			return true;
		}
		return false;
	}
};
