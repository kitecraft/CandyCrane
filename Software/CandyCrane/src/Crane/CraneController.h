#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../Utilities/IncomeingMessageQueue.h"
#include "DollyStepper.h"
#include "RopeBarrellStepper.h"
#include "TowerStepper.h"


extern IncomeingMessageQueue g_incomeingMessageQueue;

class CraneController
{
private:
	bool _QueueHandlerRunning = false;
	bool _CraneInAutoMode = false;

	bool _bucketConnected = false;
	uint32_t _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool _bucketOpenCloseComplete = false;
	portMUX_TYPE _muxBucketOpenClose;

	uint8_t _bucketHeartbeatsWaiting = 0;
	unsigned long _nextHeartbeat = 0;

	bool WaitforBucketConnect();
	bool OpenCloseBucket(int moveToAngle, int moveSpeed, bool async = false);
	bool SendBucketHeartbeat();

	RopeBarrellStepper _ropebarrel;
	DollyStepper _dolly;
	TowerStepper _tower;


	bool CalibrateBucket();

	/* stuff for testing, remove later or else....*/
	unsigned long _nextMove = 0;
	bool _bucketState = false;

public:
	bool StartUp();
	void Run();
	void RunQueueHandler();
	void SetAutoCraneStatus(bool status) { _CraneInAutoMode = status; }
	bool GetAutoCraneStatus() { return _CraneInAutoMode; }

	bool GetBucketDistance();
	bool CalibrateAll();
	void StopAll() { StopDolly(); StopTowerMotion(); StopBucketMotion(); _CraneInAutoMode = false; }

	void MoveDollyOutwards() { _dolly.MoveDollyOutwards(); }
	void MoveDollyInwards() { _dolly.MoveDollyInwards(); }
	void MoveDollyTo(int mmFromHome) { _dolly.MoveDollyTo(mmFromHome); }
	void StopDolly() { _dolly.StopDolly(); }
	bool IsDollyInMotion() { return _dolly.IsDollyInMotion(); }

	void MoveBucketDownwards() { _ropebarrel.DropBucket(); }
	void MoveBucketUpwards() { _ropebarrel.RaiseBucket(); }
	void MoveBucketTo(int mmFromHome) { _ropebarrel.MoveBucketTo(mmFromHome); }
	void StopBucketMotion() { _ropebarrel.StopBucket(); }
	bool IsBucketInMotion() { return _ropebarrel.IsBucketInMotion(); }

	void MoveTowerOutwards() { _tower.MoveTowerOutwards(); }
	void MoveTowerInwards() { _tower.MoveTowerInwards(); }
	void MoveTowerTo(int mmFromHome) { _tower.MoveTowerTo(mmFromHome); }
	void StopTowerMotion() { _tower.StopTower(); }
	bool IsTowerInMotion() { return _tower.IsTowerInMotion(); }

	bool OpenBucket() { return OpenCloseBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED); }
	void OpenBucketAsync() { OpenCloseBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED, true); }
	bool CloseBucket() { return OpenCloseBucket(DEFAULT_BUCKET_CLOSED_ANGLE, DEFAULT_BUCKET_CLOSE_SPEED); }	
	void CloseBucketAsync() { OpenCloseBucket(DEFAULT_BUCKET_CLOSED_ANGLE, DEFAULT_BUCKET_CLOSE_SPEED, true); }
};

