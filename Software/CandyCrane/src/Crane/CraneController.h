#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../Utilities/IncomeingMessageQueue.h"
#include "RopeBarrellStepper.h"
#include "../Utilities/CraneStepper.h"
#include "CandyRanger.h"

#include <ServoEasing.hpp>

extern IncomeingMessageQueue g_incomeingMessageQueue;

class CraneController
{
private:
	bool _QueueHandlerRunning = false;
	bool _CraneInAutoMode = false;

	bool _bucketConnected = false;
	bool _bucketOpenCloseComplete = false;
	portMUX_TYPE _muxBucketOpenClose;

	uint8_t _bucketHeartbeatsWaiting = 0;
	unsigned long _nextHeartbeat = 0;

	bool WaitforBucketConnect();
	bool OpenCloseBucket(int moveToAngle, int moveSpeed, bool async = false);
	bool SendBucketHeartbeat();

	RopeBarrellStepper _ropebarrel;
	CraneStepper _dolly;
	CraneStepper _tower;

	bool CalibrateBucket();

	CandyRanger _candyRanger;

	/* stuff for testing, remove later or else....*/
	unsigned long _nextMove = 0;
	bool _bucketState = false;

public:
	bool StartUp();
	void Run();
	void RunQueueHandler();
	void SetAutoCraneStatus(bool status) { _CraneInAutoMode = status; }
	bool GetAutoCraneStatus() { return _CraneInAutoMode; }

	bool CalibrateAll();
	void StopAll() { StopDolly(); StopTowerMotion(); StopBucketMotion(); _CraneInAutoMode = false; }
	void DisableAll() { _dolly.DisableStepper(); _tower.DisableStepper(); _ropebarrel.Disable(); }

	void MoveDollyOutwards() { _dolly.MoveOut(); }
	void MoveDollyInwards(){ _dolly.MoveIn(); }
	void MoveDollyTo(int mmFromHome) { _dolly.MoveToMM(mmFromHome); }
	void StopDolly() { _dolly.DeadStop(); }
	bool IsDollyInMotion() { return _dolly.IsInMotion(); }

	void MoveBucketDownwards() { _ropebarrel.DropBucket(); }
	void MoveBucketUpwards() { _ropebarrel.RaiseBucket(); }
	void MoveBucketTo(int mmFromHome) { _ropebarrel.MoveBucketTo(mmFromHome); }
	void StopBucketMotion() { _ropebarrel.StopBucket(); }
	bool IsBucketInMotion() { return _ropebarrel.IsInMotion(); }

	void MoveTowerOutwards() { _tower.MoveOut(); }
	void MoveTowerInwards() { _tower.MoveIn(); }
	void MoveTowerTo(float mmFromHome) { _tower.MoveToMM(mmFromHome); }
	void StopTowerMotion() { _tower.DeadStop(); }
	bool IsTowerInMotion() { return _tower.IsInMotion(); }

	bool OpenBucket() { return OpenCloseBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED); }
	void OpenBucketAsync() { OpenCloseBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED, true); }
	bool CloseBucket() { return OpenCloseBucket(DEFAULT_BUCKET_CLOSED_ANGLE, DEFAULT_BUCKET_CLOSE_SPEED); }	
	void CloseBucketAsync() { OpenCloseBucket(DEFAULT_BUCKET_CLOSED_ANGLE, DEFAULT_BUCKET_CLOSE_SPEED, true); }
};

