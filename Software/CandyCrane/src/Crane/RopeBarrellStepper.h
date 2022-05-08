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

public:
	RopeBarrellStepper();
	bool Init();
	void Process();

	void StopBucket();
	bool IsBucketInMotion();

	void DropBucket();
	void DropBucket(int mm);
	
	void RaiseBucket(int mm);
	void RaiseBucket();
	
	void MoveBucketTo(int mmFromHome);
	
	void SetBucketHomeAsCurrent() { _stepper->setCurrentPositionAsHome(); }
};

