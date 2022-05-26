#include "CandyRanger.h"

bool CandyRanger::Init()
{
	_servo = new ServoEasing();

	if (_servo->attach(CANDY_RANGER_SERVO_PIN, 120) == INVALID_SERVO) {
		Serial.println(F("Error attaching servo"));
		return false;
	}

	return true;
}