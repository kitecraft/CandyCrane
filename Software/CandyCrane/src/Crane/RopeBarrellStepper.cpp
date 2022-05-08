#include "RopeBarrellStepper.h"


RopeBarrellStepper::RopeBarrellStepper()
{
}

bool RopeBarrellStepper::Init()
{
	_muxBucketMotion = portMUX_INITIALIZER_UNLOCKED;
	_ropeBarrelMaxiumSteps = RopeBarrelStepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepper = new CheapStepper(ROPE_BARREL_STEPPER_PIN_1, ROPE_BARREL_STEPPER_PIN_2, ROPE_BARREL_STEPPER_PIN_3, ROPE_BARREL_STEPPER_PIN_4);
	_stepper->setRpm(20);
	return true;
}
void RopeBarrellStepper::Process()
{
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
	DropBucket(ROPE_BARREL_MAXIMUM_DISTANCE);
}

void RopeBarrellStepper::DropBucket(int mm)
{
	int numberOfStepsToMove = RopeBarrelStepsForDistance(mm);
	_stepper->newMoveCW(numberOfStepsToMove);
	SetBucketMotionStatus(true);
}

void RopeBarrellStepper::RaiseBucket()
{
	RaiseBucket(ROPE_BARREL_MAXIMUM_DISTANCE);
}

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


void RopeBarrellStepper::StopBucket()
{
	_stepper->stop();
}