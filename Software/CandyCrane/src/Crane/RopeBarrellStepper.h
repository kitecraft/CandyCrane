#pragma once
#include <Arduino.h>
//#include <CheapStepper.h>
#include "../Utilities/CraneStepper.h"
#include "../CC_Config.h"

class RopeBarrellStepper
{
private:
	//CheapStepper* _stepper;
	//bool _isBucketInMotion = false;
	//portMUX_TYPE _muxBucketMotion;
	//int _ropeBarrelMaxiumSteps = 0;
	//void SetBucketMotionStatus(bool status);
	//int RopeBarrelStepsForDistance(double distance) { return int(distance * (double)ROPE_BARREL_STEPS_PER_MM); }

	CraneStepper _stepper;
	float _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool FetchBucketDistance();


public:
	RopeBarrellStepper();
	bool Init();
	void SetBucketDistance(uint16_t distance);
	void Process();

	void StopBucket();
	bool IsInMotion() { return _stepper.IsInMotion(); }

	void DropBucket();
	void RaiseBucket();
	void MoveBucketTo(int mmFromHome);

	bool Calibrate();
	void Disable() { _stepper.DisableStepper(); }

	//void DropBucket(int mm);
	//void RaiseBucket(int mm);
	//void SetBucketHomeAsCurrent() { _stepper->setCurrentPositionAsHome(); }
};

