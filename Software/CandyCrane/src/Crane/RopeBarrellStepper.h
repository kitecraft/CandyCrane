#pragma once
#include <Arduino.h>
#include "../Utilities/CraneStepper.h"
#include "../CC_Config.h"


typedef std::function<void()> requestDistanceFunc;
class RopeBarrellStepper
{
private:
	float _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool FetchBucketDistance();
	requestDistanceFunc requestDistance = nullptr;


public:
	RopeBarrellStepper();
	bool Init();
	void SetBucketDistance(int distance);

	void StopBucket() {  }
	bool IsInMotion() {  }

	void DropBucket() {  }  //{ _stepper.MoveOut(); }
	void RaiseBucket() {  } //{ _stepper.MoveIn(); }

	void MoveBucketTo(int mmFromHome) {  }

	bool Calibrate();
	void Disable() {  }

	void SetCurrentPosition(long pos) {  }
	void MoveInMM(float mm) { }
	void MoveOutMM(float mm) {  }
	void SetCurrentPositionAsHome() {  }

	void setCallback(requestDistanceFunc fn) { requestDistance = fn; }
};

