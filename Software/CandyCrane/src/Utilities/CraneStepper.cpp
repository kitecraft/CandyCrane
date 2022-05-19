#include "CraneStepper.h"

CraneStepper::CraneStepper()
{

}

void CraneStepper::Init(byte stepperPin1, byte stepperPin2, byte stepperPin3, byte stepperPin4)
{
	_muxMotion = portMUX_INITIALIZER_UNLOCKED;
	_stepper.connectToPins(stepperPin1, stepperPin2, stepperPin3, stepperPin4);
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
	if (_stepper.getCurrentPositionInSteps() > GetMaximumStep())
	{
		Serial.println("Maximum Limit Reached");
		return SME_MAXIMUM_TRAVEL_LIMIT_REACHED;
	}
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::IsMiniumLimitReached()
{
	if (_usingLimitSwitch) {
		if (IsLimitSwitchActive()) {
			Serial.println("Limit switch active");
			return SME_LIMIT_SWITCH_ACTIVE;
		}
	}
	else {
		if (_stepper.getCurrentPositionInSteps() <= 0) {
			Serial.println("Minimum Limit Reached");
			return SME_MINIMUM_TRAVEL_LIMIT_REACHED;
		}
	}
	return SME_NONE;
}

bool CraneStepper::Calibrate(int stepsPerSec)
{
	if (!_usingLimitSwitch) {
		return false;
	}
	return _stepper.moveToHomeInSteps(DIRECTION_IN, stepsPerSec, GetMaximumStep() + 20, _limitSwitchPin);
}

STEPPER_MOVE_ERROR CraneStepper::MoveOut()
{
	Serial.println("Moving this outwards");
	STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	_direction = DIRECTION_OUT;
	_stepper.setTargetPositionInSteps(GetMaximumStep());
	SetMotionStatus(true);
	Serial.printf("Current: %i to: %i\n", _stepper.getCurrentPositionInSteps(), GetMaximumStep());
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
	SetMotionStatus(true);
	_stepper.setTargetPositionInSteps(_stepper.getCurrentPositionInSteps() + steps);
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::MoveIn()
{
	/*
	STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
	if (state != SME_NONE) {
		return state;
	}
	*/
	_direction = DIRECTION_IN;
	Serial.printf("Current: %i to: %i\n", _stepper.getCurrentPositionInSteps(), 0);
	_stepper.setTargetPositionInSteps(-50000);
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
	Serial.printf("Moving in %i steps.\n", steps);
	_stepper.setTargetPositionInSteps(_stepper.getCurrentPositionInSteps() - steps);
	SetMotionStatus(true);
	return SME_NONE;
}


STEPPER_MOVE_ERROR CraneStepper::MoveToMM(float mm)
{
	long stepToMoveTo = StepsForDistance(mm) - _stepper.getCurrentPositionInSteps();
	if (stepToMoveTo > 0){
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

	_stepper.setTargetPositionInSteps(stepToMoveTo);
	SetMotionStatus(true);
	return SME_NONE;
}

STEPPER_MOVE_ERROR CraneStepper::Process()
{
	if (IsInMotion())
	{
		/*
		if (_direction == DIRECTION_OUT) {
			STEPPER_MOVE_ERROR state = IsMaximumLimitReached();
			if (state != SME_NONE) {
				_stepper.deadStop();
				SetMotionStatus(false);
				return state;
			}
		}
		else {
			STEPPER_MOVE_ERROR state = IsMiniumLimitReached();
			if (state != SME_NONE) {
				_stepper.deadStop();
				SetMotionStatus(false);
				return state;
			}
		}
		*/
		if (!_stepper.motionComplete())
		{
			_stepper.processMovement();
		}
		else {
			SetMotionStatus(false);
		}
	}
	return SME_NONE;
}
