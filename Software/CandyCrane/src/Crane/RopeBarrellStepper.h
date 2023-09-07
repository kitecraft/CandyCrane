#pragma once
#include <Arduino.h>
#include "../MultiTinyStepper/MultiTinyStepper.h"
#include "../MultiTinyStepper/MtsStepper.h"
#include "../Utilities/CraneStepper.h"
#include "../CC_Config.h"


typedef std::function<void()> requestDistanceFunc;
class RopeBarrellStepper
{
private:
	CraneStepper _stepperR;
	CraneStepper _stepperL;
	float _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool FetchBucketDistance();
	requestDistanceFunc requestDistance = nullptr;


public:
	RopeBarrellStepper();
	bool Init(MtsStepper* stepperR, MtsStepper* stepperL);
	void SetBucketDistance(int distance);

	void StopBucket() { _stepperR.DeadStop(); _stepperL.DeadStop(); }
	bool IsInMotion() { return _stepperR.IsInMotion() && _stepperL.IsInMotion(); }

	void DropBucket() { _stepperR.MoveOutMM(30); _stepperL.MoveOutMM(30); }  //{ _stepper.MoveOut(); }
	void RaiseBucket() { _stepperR.MoveInMM(30); _stepperL.MoveInMM(30); } //{ _stepper.MoveIn(); }

	void MoveBucketTo(int mmFromHome) { _stepperR.MoveToMM(mmFromHome); _stepperL.MoveToMM(mmFromHome); }

	bool Calibrate();
	void Disable() { _stepperR.DisableStepper(); _stepperL.DisableStepper(); }

	void SetCurrentPosition(long pos) { _stepperR.SetCurrentPosition(pos); _stepperL.SetCurrentPosition(pos); }
	void MoveInMM(float mm) { _stepperR.MoveInMM(mm); _stepperL.MoveInMM(mm); }
	void MoveOutMM(float mm) { _stepperR.MoveOutMM(mm); _stepperL.MoveOutMM(mm); }
	void SetCurrentPositionAsHome() { _stepperR.SetCurrentPositionAsHome(); _stepperL.SetCurrentPositionAsHome(); }

	void setCallback(requestDistanceFunc fn) { requestDistance = fn; }
};

