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

	//_stepper.SetCurrentPoistion(500000);

	/*
	_muxBucketMotion = portMUX_INITIALIZER_UNLOCKED;
	_ropeBarrelMaxiumSteps = RopeBarrelStepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepper = new CheapStepper(ROPE_BARREL_STEPPER_PIN_1, ROPE_BARREL_STEPPER_PIN_2, ROPE_BARREL_STEPPER_PIN_3, ROPE_BARREL_STEPPER_PIN_4);
	_stepper->setRpm(20);
	*/
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
		//_ropebarrel.DropBucket(distanceToMove);
	}
	else {
		_stepper.MoveOutMM(distanceToMove * -1);
		//_ropebarrel.RaiseBucket(distanceToMove * -1);
	}

	//while (_ropebarrel.IsBucketInMotion())
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


	/*
void RopeBarrellStepper::Process()
{
	_stepper.Process();
	if (IsBucketInMotion())
	{
		if (_stepper->getStepsLeft() != 0)
		{
			if (_stepper->getStep() < _ropeBarrelMaxiumSteps + 1)
			{
				_stepper->run();
			}
			else {
				Serial.println("RopeBarrel: Bucket: Ending run due to a limit being hit");
				_stepper->stop();
				SetBucketMotionStatus(false);
			}
		}
		else {
			Serial.printf("RopeBarrel: Bucket Move complete. Current Step: %i\n", _stepper->getStep());
			SetBucketMotionStatus(false);
		}
	}
}
	*/

/*
void RopeBarrellStepper::SetBucketMotionStatus(bool status)
{
	portENTER_CRITICAL(&_muxBucketMotion);
	_isBucketInMotion = status;
	portEXIT_CRITICAL(&_muxBucketMotion);
}

bool RopeBarrellStepper::IsBucketInMotion()
{
	portENTER_CRITICAL(&_muxBucketMotion);
	bool ret = _isBucketInMotion;
	portEXIT_CRITICAL(&_muxBucketMotion);

	return ret;
}
*/
void RopeBarrellStepper::DropBucket()
{
	_stepper.MoveOut();
	//DropBucket(ROPE_BARREL_MAXIMUM_DISTANCE);
}

/*
void RopeBarrellStepper::DropBucket(int mm)
{
	int numberOfStepsToMove = RopeBarrelStepsForDistance(mm);
	_stepper->newMoveCW(numberOfStepsToMove);
	SetBucketMotionStatus(true);
}
*/

void RopeBarrellStepper::RaiseBucket()
{
	_stepper.MoveIn();
	//RaiseBucket(ROPE_BARREL_MAXIMUM_DISTANCE);
}

void RopeBarrellStepper::MoveBucketTo(int mmFromHome)
{
	_stepper.MoveToMM(mmFromHome);
}


/*
void RopeBarrellStepper::RaiseBucket(int mm)
{
	int numberOfStepsToMove = RopeBarrelStepsForDistance(mm);
	_stepper->newMoveCCW(numberOfStepsToMove);  // moves x steps to 0 where is is the current step count
	SetBucketMotionStatus(true);
}


void RopeBarrellStepper::MoveBucketTo(int mmFromHome)
{
	int stepToMoveTo = RopeBarrelStepsForDistance(mmFromHome);
	int stepsToMove = stepToMoveTo - _stepper->getStep();
	if (stepsToMove > 0)
	{
		_stepper->newMoveCW(stepsToMove);
	}
	else {
		_stepper->newMoveCCW(stepsToMove);
	}
	SetBucketMotionStatus(true);
}
*/


void RopeBarrellStepper::StopBucket()
{
	_stepper.DeadStop();
	//_stepper->stop();
}