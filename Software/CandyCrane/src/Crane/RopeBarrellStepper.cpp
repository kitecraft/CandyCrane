#include "RopeBarrellStepper.h"
//#include "../ESPNow/EspNowMessage.h"

RopeBarrellStepper::RopeBarrellStepper()
{
}

bool RopeBarrellStepper::Init(MtsStepper* stepperR, MtsStepper* stepperL)
{
	_muxBucketDistance = portMUX_INITIALIZER_UNLOCKED;

	_stepperR.Init(stepperR);
	_stepperR.SetReversed(ROPE_BARREL_STEPPER_R_DIR);
	_stepperR.SetStepsPerMM(ROPE_BARREL_STEPS_PER_MM);
	_stepperR.SetMaximumDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepperR.SetSpeed(ROPE_BARREL_SPEED_STEPS_SECOND);
	_stepperR.SetAcceleration(ROPE_BARREL_ACCEL_STEPS_SECOND);

	_stepperL.Init(stepperL);
	_stepperL.SetReversed(ROPE_BARREL_STEPPER_L_DIR);
	_stepperL.SetStepsPerMM(ROPE_BARREL_STEPS_PER_MM);
	_stepperL.SetMaximumDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepperL.SetSpeed(ROPE_BARREL_SPEED_STEPS_SECOND);
	_stepperL.SetAcceleration(ROPE_BARREL_ACCEL_STEPS_SECOND);

	return true;
}

/*
Blocks until distance is returned
*/
bool RopeBarrellStepper::FetchBucketDistance()
{
	_bucketDistance = (float)BUCKET_NO_DISTANCE;
	
	requestDistance();

	// wait for distance to be updated
	unsigned long endWait = millis() + BUCKET_COMMAND_WAIT_TIME;
	while (millis() < endWait) {
		portENTER_CRITICAL(&_muxBucketDistance);
		float distance = _bucketDistance;
		portEXIT_CRITICAL(&_muxBucketDistance);

		if (distance != BUCKET_NO_DISTANCE)
		{
			Serial.print("FetchBucketDistance: Bucket distance is: ");
			Serial.println(distance);
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
void RopeBarrellStepper::SetBucketDistance(int distance)
{
	portENTER_CRITICAL(&_muxBucketDistance);
	_bucketDistance = (float)distance/100.0;
	portEXIT_CRITICAL(&_muxBucketDistance);
}

bool RopeBarrellStepper::Calibrate()
{
	Serial.println("\nStarting Rope barrell calibration");
	
	if (!FetchBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance.");
		return false;
	}

	Serial.print("Got distance: ");
	Serial.println(_bucketDistance);
	while (_bucketDistance < 60) {
		MoveOutMM(20);
		while (IsInMotion())
		{
			_stepperR.Process();
			_stepperL.Process();
			vTaskDelay(1);
		}
		FetchBucketDistance();
	}
	

	SetCurrentPosition(_stepperR.StepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE) * 2);
	float distanceToMove = _bucketDistance - BUCKET_DEFAULT_DISTANCE;

	Serial.printf("Moving bucket %f mm\n", distanceToMove);
	if (distanceToMove > 0) {
		MoveInMM(distanceToMove);
	}
	else {
		MoveOutMM(distanceToMove * -1);
	}

	while(IsInMotion())
	{
		_stepperR.Process();
		_stepperL.Process();
		vTaskDelay(1);
	}
	if (!FetchBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance After Move.");
		return false;
	}
	Serial.printf("Calibrate(): Final Bucket distance is: '%f'\n\n\n--------\n", _bucketDistance);
	SetCurrentPositionAsHome();
	
	return true;
}