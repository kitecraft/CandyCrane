#pragma once
#include <Arduino.h>
#include <CheapStepper.h>
#include "../CC_Config.h"

class RopeBarrellStepper
{
private:
	CheapStepper* _stepper;
	bool _isBucketInMotion = false;
	portMUX_TYPE _muxBucketMotion;
	int _ropeBarrelMaxiumSteps = 0;
	void SetBucketMotionStatus(bool status);
	int RopeBarrelStepsForDistance(double distance) { return int(distance * (double)ROPE_BARREL_STEPS_PER_MM); }

	bool _errorCondition = false;
public:
	RopeBarrellStepper();
	bool Init();
	bool Calibrate();

	void Process();
	void DropBucket();
	void RaiseBucket();
	void StopBucket();
	bool IsBucketInMotion();
};

