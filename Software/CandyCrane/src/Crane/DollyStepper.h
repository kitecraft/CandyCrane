#pragma once
#include <Arduino.h>
#include <CheapStepper.h>
#include "../CC_Config.h"

class DollyStepper
{
private:
	CheapStepper *_stepper;
	int _dollyMaxiumSteps = 0;
	bool _isDollyInMotion = false;
	portMUX_TYPE _muxDollyMotion;
	void SetDollyMotionStatus(bool status);
	bool IsDollyLimitSwitchActive();
	int DollyStepsForDistance(double distance) { return int(distance * (double)DOLLY_STEPS_PER_MM); }

	bool _errorCondition = false;

public:
	DollyStepper();
	bool Init();
	bool Calibrate();

	void Process();
	void MoveDollyOutwards();
	void MoveDollyInwards();
	void StopDolly();
	bool IsDollyInMotion();
};

