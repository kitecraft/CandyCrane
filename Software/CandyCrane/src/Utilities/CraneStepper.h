#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../Utilities/SmallFlexyStepper.h"

#define DIRECTION_OUT 1
#define DIRECTION_IN -1


class CraneStepper
{
private:
	SmallFlexyStepper* _stepper;

	bool _isInMotion = false;
	portMUX_TYPE _muxMotion;

	bool _usingLimitSwitch = false;
	byte _limitSwitchPin = -1;

	float _stepsPerMM = 0;
	float _maximumDistance = 0;
	long _maxiumSteps = 0;
	byte _direction = DIRECTION_OUT;

public:
	CraneStepper();
	void Init(byte inPin1, byte inPin2, byte inPin3, byte inPin4);
	void SetSpeed(float stepsPerSecond) { _stepper->setSpeedInStepsPerSecond(stepsPerSecond); }
	void SetAcceleration(float stepsPerSecond) { _stepper->setAccelerationInStepsPerSecondPerSecond(stepsPerSecond); }
	void ConnectToLimitSwitch(byte pinNumber);
	void SetMotionStatus(bool status);
	bool IsInMotion();

	void SetStepsPerMM(double steps) { _stepsPerMM = steps;  }
	void SetMaximumDistance(int mmFromHome) { _maximumDistance = StepsForDistance(mmFromHome); }
	long GetMaximumStep() { return _maximumDistance; }
	long StepsForDistance(float distance) { return round(distance * _stepsPerMM); }

	float GetCurrentPosition() { return _stepper->getCurrentPositionInSteps(); }

	void SetCurrentPositionAsHome() { _stepper->setCurrentPositionInSteps(0); }
	void SetCurrentPosition(long pos) { _stepper->setCurrentPositionInSteps(pos); }
	bool IsLimitSwitchActive() { return ((digitalRead(_limitSwitchPin) == LOW) ? true : false); }
	void DisableStepper() { _stepper->disable(); }
	STEPPER_MOVE_ERROR IsMaximumLimitReached();
	STEPPER_MOVE_ERROR IsMiniumLimitReached();


	bool Calibrate(int stepsPerSec);
	STEPPER_MOVE_ERROR MoveOut();
	STEPPER_MOVE_ERROR MoveOutMM(float mmToMoveOutwards);
	STEPPER_MOVE_ERROR MoveIn();
	STEPPER_MOVE_ERROR MoveInMM(float mmToMoveInwards);
	STEPPER_MOVE_ERROR MoveToMM(float mm);
	void DeadStop() { _stepper->deadStop(); }

	STEPPER_MOVE_ERROR Process();
};

