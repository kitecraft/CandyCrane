#include "CandyRanger.h"

bool CandyRanger::Init()
{
	if (!InitRanger()) {
		return false;
	}

	if (!InitServo()) {
		return false;
	}

	return true;
}

bool CandyRanger::InitRanger()
{
	if (!_ranger.init())
	{
		Serial.println("Failed to detect and initialize sensor!  As such, I will now refuse to continue.");
		return false;
		//while (true) { delay(1); }
	}
	_ranger.setTimeout(500);
	_ranger.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	_ranger.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
	_ranger.setMeasurementTimingBudget(200000);
	if (!_ranger.setSignalRateLimit(0.75)) {
		Serial.println("setSignalRateLimit was not valid");
	}

	return true;
}

bool CandyRanger::InitServo()
{
		if (_servo.attach(CANDY_RANGER_SERVO_PIN, 80) == INVALID_SERVO) {
			Serial.println(F("Error attaching servo"));
			return false;
		}
	_servo.easeTo(CANDY_RANGER_HOME_POS, CANDY_RANGER_FAST_SPEED);

	int val = GetDistanceForCandyA();
	Serial.printf("Candy A average distance is: %d\n", val);
	delay(1000);

	val = GetDistanceForCandyB();
	Serial.printf("Candy B average distance is: %d\n", val);

	return true;
}

int CandyRanger::GetDistanceForCandy(CANDY_ID candyId)
{
	int startPos = 0;
	int endPos = 0;

	switch (candyId) {
	case CANDY_A:
		startPos = CANDY_RANGER_ZONE_A_START;
		endPos = CANDY_RANGER_ZONE_A_END;
		break;
	case CANDY_B:
		startPos = CANDY_RANGER_ZONE_B_START;
		endPos = CANDY_RANGER_ZONE_B_END;
		break;
	default:
		return 0;
	}

	int space = (endPos - startPos) / CANDY_RANGER_NUM_SAMPLES-1;
	int total = 0;
	int currPos = startPos;
	_servo.easeTo(currPos, CANDY_RANGER_FAST_SPEED);
	for (int i = 0; i < CANDY_RANGER_NUM_SAMPLES; i++) {
		int m = _ranger.readRangeSingleMillimeters();
		total += m;
		//Serial.printf("Measurement %d is %d\n", i, m);
		currPos += space;
		if (currPos > endPos) {
			currPos = endPos;
		}
		_servo.easeTo(currPos, CANDY_RANGER_FAST_SPEED);
	}

	_servo.easeTo(CANDY_RANGER_HOME_POS, CANDY_RANGER_FAST_SPEED);
	return total / CANDY_RANGER_NUM_SAMPLES;
}

int CandyRanger::GetDistanceForCandyA()
{
	return GetDistanceForCandy(CANDY_A);
}

int CandyRanger::GetDistanceForCandyB()
{
	return GetDistanceForCandy(CANDY_B);
}