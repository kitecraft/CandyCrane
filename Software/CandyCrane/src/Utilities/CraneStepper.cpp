#include "CraneStepper.h"

CraneStepper::CraneStepper()
{

}

void CraneStepper::Init(MtsStepper* stepper)
{
	_muxMotion = portMUX_INITIALIZER_UNLOCKED;
	_stepper = stepper;
}

void CraneStepper::ConnectToLimitSwitch(byte pinNumber)
{
	_limitSwitchPin = pinNumber;
	pinMode(_limitSwitchPin, INPUT_PULLUP);
	_usingLimitSwitch = true;
}

void CraneStepper::SetMotionStatus(bool status)
{
	portENTER_CRITICAL(&_muxMotion);
	_isInMotion = status;
	portEXIT_CRITICAL(&_muxMotion);
}

bool CraneStepper::IsInMotion()
{
	portENTER_CRITICAL(&_muxMotion);
	bool ret = _isInMotion;
	portEXIT_CRITICAL(&_muxMotion);

	return ret;
}

STEPPER_MOVE_ERROR CraneStepper::IsMaximumLimitReached()
{
	if (_stepper->getCurrentPositionInSteps() > GetMaximumStep())
	{
		//Serial.println("Maximum Limit Reached");
		return SME_MAXIMUM_TRAVEL_LIMIT_REACHED;
	}
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::IsMiniumLimitReached()
{
	if (_usingLimitSwitch) {
		if (IsLimitSwitchActive()) {
			//Serial.println("Limit switch active");
			return SME_LIMIT_SWITCH_ACTIVE;
		}
	}
	else {
		if (_stepper->getCurrentPositionInSteps() <= 0) {
			//Serial.println("Minimum Limit Reached");
			return SME_MINIMUM_TRAVEL_LIMIT_REACHED;
		}
	}
	return SME_NONE;
}

bool CraneStepper::Calibrate(int stepsPerSec)
{
	_stepper->setCurrentPositionInSteps(GetMaximumStep() + 256);

	if (!_usingLimitSwitch) {
		return false;
	}

	if (!_stepper->moveToHomeInSteps(DIRECTION_IN, stepsPerSec, GetMaximumStep() + 256, _limitSwitchPin))
	{
		return false;
	}

	_stepper->setTargetPositionRelativeInSteps(StepsForDistance(MM_FROM_SWITCH_FOR_HOME));
	_stepper->setCurrentPositionInSteps(0);
	return true;
}

STEPPER_MOVE_ERROR CraneStepper::MoveOut()
{
	//Serial.println("Moving this outwards");
	STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	_direction = DIRECTION_OUT;
	_stepper->setTargetPositionInSteps(GetMaximumStep());
	SetMotionStatus(true);
	//Serial.printf("Current: %i to: %i\n", _stepper->getCurrentPositionInSteps(), GetMaximumStep());
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::MoveOutMM(float mmToMoveOutwards)
{
	STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	_direction = DIRECTION_OUT;
	long steps = StepsForDistance(mmToMoveOutwards);
	_stepper->setTargetPositionInSteps(_stepper->getCurrentPositionInSteps() + steps);
	SetMotionStatus(true);
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::MoveIn()
{
	STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	_direction = DIRECTION_IN;
	//Serial.println("Moving this inwards");
	//Serial.printf("Current: %i to: %i\n", _stepper->getCurrentPositionInSteps(), 0);
	_stepper->setTargetPositionInSteps(1);
	SetMotionStatus(true);
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::MoveInMM(float mmToMoveInwards)
{
	STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	long steps = StepsForDistance(mmToMoveInwards);
	_direction = DIRECTION_IN;
	//Serial.printf("Moving in %i steps.\n", steps);
	_stepper->setTargetPositionInSteps(_stepper->getCurrentPositionInSteps() - steps);
	SetMotionStatus(true);
	return SME_NONE;
}


STEPPER_MOVE_ERROR CraneStepper::MoveToMM(float mm)
{
	long stepToMoveTo = StepsForDistance(mm);
	long numberStepsToMove = stepToMoveTo - _stepper->getCurrentPositionInSteps();
	if (numberStepsToMove > 0){
		STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
		if (state != SME_NONE) {
			return state;
		}
		_direction = DIRECTION_OUT;
	} else {
		STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
		if (state != SME_NONE) {
			return state;
		}
		_direction = DIRECTION_IN;
	}

	_stepper->setTargetPositionInSteps(stepToMoveTo);
	SetMotionStatus(true);
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::Process()
{
	if (IsInMotion())
	{
		if (_direction == DIRECTION_OUT) {
			STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
			if (state != SME_NONE) {
				_stepper->deadStop();
				SetMotionStatus(false);
				return state;
			}
		}
		else {
			STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
			if (state != SME_NONE) {
				_stepper->deadStop();
				SetMotionStatus(false);
				//Serial.printf("Ending process due to minimium limit reached. Steps: %i\n", _stepper->getCurrentPositionInSteps());
				
				return state;
			}
		}

		if (!_stepper->motionComplete())
		{
			_stepper->process();
		}
		else {
			//Serial.printf("Motion is complete. Steps: %i\n", _stepper->getCurrentPositionInSteps());
			SetMotionStatus(false);
		}
	}
	return SME_NONE;
}
