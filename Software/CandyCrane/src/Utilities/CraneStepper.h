#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../SmallFlexyStepper/SmallFlexyStepper.h"

#define DIRECTION_OUT 1
#define DIRECTION_IN -1

enum STEPPER_MOVE_ERROR {
	SME_NONE = 0,
	SME_LIMIT_SWITCH_ACTIVE,
	SME_MINIMUM_TRAVEL_LIMIT_REACHED,
	SME_MAXIMUM_TRAVEL_LIMIT_REACHED
};

class CraneStepper
{
private:
	SmallFlexyStepper _stepper;

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
	void Init(byte stepperPin1, byte stepperPin2, byte stepperPin3, byte stepperPin4);
	void SetSpeed(float stepsPerSecond) { _stepper.setSpeedInStepsPerSecond(stepsPerSecond); }
	void SetAcceleration(float stepsPerSecond) { _stepper.setAccelerationInStepsPerSecondPerSecond(stepsPerSecond); }
	void ConnectToLimitSwitch(byte pinNumber);
	void SetMotionStatus(bool status);
	bool IsInMotion();
	void SetStepsPerMM(double steps) { _stepsPerMM = steps;  }
	void SetMaximumDistance(byte mmFromHome) { _maximumDistance = StepsForDistance(mmFromHome); }
	float GetCurrentPosition() { return _stepper.getCurrentPositionInSteps(); }
	long GetMaximumStep() { return _maximumDistance; }
	long StepsForDistance(float distance) { return round(distance * _stepsPerMM); }
	bool IsLimitSwitchActive() { return ((digitalRead(_limitSwitchPin) == LOW) ? true : false); }
	void DisableStepper() { _stepper.disable(); }
	STEPPER_MOVE_ERROR IsMaximumLimitReached();
	STEPPER_MOVE_ERROR IsMiniumLimitReached();

	bool Calibrate();
	STEPPER_MOVE_ERROR MoveOut();
	STEPPER_MOVE_ERROR MoveIn();
	STEPPER_MOVE_ERROR MoveToMM(float mm);
	void DeadStop() { _stepper.deadStop(); }

	STEPPER_MOVE_ERROR Process();
};

