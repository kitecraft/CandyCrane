#include "RopeBarrellStepper.h"
#include "../ESPNow/EspNowMessage.h"

RopeBarrellStepper::RopeBarrellStepper()
{
}

bool RopeBarrellStepper::Init()
{
	_muxBucketDistance = portMUX_INITIALIZER_UNLOCKED;

	_stepper.Init(ROPE_BARREL_STEPPER_PIN_1, ROPE_BARREL_STEPPER_PIN_2, ROPE_BARREL_STEPPER_PIN_3, ROPE_BARREL_STEPPER_PIN_4);
	_stepper.SetStepsPerMM(ROPE_BARREL_STEPS_PER_MM);
	_stepper.SetMaximumDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepper.SetSpeed(ROPE_BARREL_SPEED_STEPS_SECOND);
	_stepper.SetAcceleration(ROPE_BARREL_ACCEL_STEPS_SECOND);

	return true;
}

/*
Blocks until distance is returned
*/
bool RopeBarrellStepper::FetchBucketDistance()
{
	_bucketDistance = BUCKET_NO_DISTANCE;
	SendCommand(CC_GET_DISTANCE);

	// wait for distance to be updated
	unsigned long endWait = millis() + BUCKET_COMMAND_WAIT_TIME;
	while (millis() < endWait) {
		portENTER_CRITICAL(&_muxBucketDistance);
		uint32_t distance = _bucketDistance;
		portEXIT_CRITICAL(&_muxBucketDistance);

		if (distance != BUCKET_NO_DISTANCE)
		{
			Serial.printf("Bucket distance is: '%i'\n", distance);
			return true;
		}
		vTaskDelay(5);
	}
	return false;
}

/*
Expects an int representing a float with 2 decimal places.
SetBucketDistance(100); = distance of 1.00 mm
*/
void RopeBarrellStepper::SetBucketDistance(uint16_t distance)
{
	portENTER_CRITICAL(&_muxBucketDistance);
	_bucketDistance = (float)distance/100.0;
	portEXIT_CRITICAL(&_muxBucketDistance);
}

bool RopeBarrellStepper::Calibrate()
{
	if (!FetchBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance.");
		return false;
	}


	while (_bucketDistance < 60) {
		_stepper.MoveOutMM(20);
		while (_stepper.IsInMotion())
		{
			_stepper.Process();
			vTaskDelay(1);
		}
		FetchBucketDistance();
	}

	_stepper.SetCurrentPoistion(_stepper.StepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE) * 2);
	float distanceToMove = _bucketDistance - BUCKET_DEFAULT_DISTANCE;

	Serial.printf("Moving bucket %f mm\n", distanceToMove);
	if (distanceToMove > 0) {
		_stepper.MoveInMM(distanceToMove);
	}
	else {
		_stepper.MoveOutMM(distanceToMove * -1);
	}

	while(_stepper.IsInMotion())
	{
		_stepper.Process();
		vTaskDelay(1);
	}
	if (!FetchBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance After Move.");
		return false;
	}
	Serial.printf("Bucket distance is: '%f'\n\n\n--------\n", _bucketDistance);
	_stepper.SetCurrentPositionAsHome();
	
	return true;
}