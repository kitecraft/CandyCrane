#pragma once
#include <Arduino.h>
//#include <CheapStepper.h>
#include "../Utilities/CraneStepper.h"
#include "../CC_Config.h"

class RopeBarrellStepper
{
private:
	CraneStepper _stepper;
	float _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool FetchBucketDistance();


public:
	RopeBarrellStepper();
	bool Init();
	void SetBucketDistance(uint16_t distance);
	void Process() { _stepper.Process(); }

	void StopBucket() { _stepper.DeadStop(); }
	bool IsInMotion() { return _stepper.IsInMotion(); }

	void DropBucket() { _stepper.MoveOutMM(30); }  //{ _stepper.MoveOut(); }
	void RaiseBucket() { _stepper.MoveInMM(30); } //{ _stepper.MoveIn(); }
	void MoveBucketTo(int mmFromHome) { _stepper.MoveToMM(mmFromHome); }

	bool Calibrate();
	void Disable() { _stepper.DisableStepper(); }
};

