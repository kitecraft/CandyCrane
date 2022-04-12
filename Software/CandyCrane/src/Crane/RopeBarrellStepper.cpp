#include "RopeBarrellStepper.h"


RopeBarrellStepper::RopeBarrellStepper()
{
}

bool RopeBarrellStepper::Init()
{
	_muxBucketMotion = portMUX_INITIALIZER_UNLOCKED;
	_ropeBarrelMaxiumSteps = RopeBarrelStepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepper = new CheapStepper(ROPE_BARREL_STEPPER_PIN_1, ROPE_BARREL_STEPPER_PIN_2, ROPE_BARREL_STEPPER_PIN_3, ROPE_BARREL_STEPPER_PIN_4);
	_stepper->setRpm(14);
	return true;
}

bool RopeBarrellStepper::Calibrate()
{

	return true;
}

void RopeBarrellStepper::Process()
{
	if (IsBucketInMotion())
	{
		if (_errorCondition)
		{
			Serial.println("RopeBarrel: Error condition exists.  Unable to move dolly.");
			_stepper->stop();
			SetBucketMotionStatus(false);
			return;
		}

		if (_stepper->getStepsLeft() != 0)
		{
			/*
			if (_stepper->getStep() < _ropeBarrelMaxiumSteps + 1)
			{
				_stepper->run();
			}
			else {
				Serial.println("RopeBarrel: Bucket: Ending run due to a limit being hit");
				_stepper->stop();
				SetBucketMotionStatus(false);
				_errorCondition = true;
			}
			*/
			_stepper->run();
		}
		else {
			Serial.println("RopeBarrel: Bucket Move complete.");
			SetBucketMotionStatus(false);
		}
	}
}


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

void RopeBarrellStepper::DropBucket()
{
	if (_errorCondition) {
		Serial.println("RopeBarrel: Error condition exists.  Unable to move rope barrel.");
		return;
	}

	
	Serial.printf("RopeBarrel: Moving Bucket Downwards '%i' steps\n", _ropeBarrelMaxiumSteps);
	_stepper->newMoveCW(_ropeBarrelMaxiumSteps);
	SetBucketMotionStatus(true);

}

void RopeBarrellStepper::RaiseBucket()
{
	if (_errorCondition) {
		Serial.println("RopeBarrel: Error condition exists.  Unable to move rope barrel.");
		return;
	}
	Serial.printf("RopeBarrel: Moving Bucket Upwards '%i' steps\n", _ropeBarrelMaxiumSteps);
	_stepper->newMoveCCW(_ropeBarrelMaxiumSteps);  // moves x steps to 0 where is is the current step count
	SetBucketMotionStatus(true);
}

void RopeBarrellStepper::StopBucket()
{
	Serial.println("RopeBarrel: Stop the Bucket, It's time to take a dump");
	_stepper->stop();
}