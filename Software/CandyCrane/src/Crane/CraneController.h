#pragma once
#include <Arduino.h>
#include <WiFiServer.h>
#include "../CC_Config.h"
#include "RopeBarrellStepper.h"
#include "../Utilities/CraneStepper.h"
#include "CandyRanger.h"
#include "CandyCraneCommands.h"

static TaskHandle_t _tcpServerThreadHandle = nullptr;

class CraneController
{
private:
	bool _CraneInAutoMode = false;

	WiFiServer* _tcpServer = nullptr;
	WiFiClient _bucketClient;
	bool _bucketConnected = false;
	bool _bucketOpenCloseComplete = false;
	portMUX_TYPE _muxBucketOpenClose;
	void SendMessageToBucket(CANDY_CRANE_COMMANDS command, int distance = 0, int angle = 0, int speed = 0);
	void RequestDistance() { SendMessageToBucket(CC_GET_DISTANCE); }
	void Ping();
	uint32_t _lastPing = 0;
	int _pingCount = 0;

	void WaitforBucketConnect();
	bool OpenCloseBucket(int moveToAngle, int moveSpeed, bool async = false);
	void ParseMessage(String message);

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
	void RunTCPServer();

	void SetAutoCraneStatus(bool status) { _CraneInAutoMode = status; }
	bool GetAutoCraneStatus() { return _CraneInAutoMode; }

	bool CalibrateAll();
	void StopAll() { StopDolly(); StopTowerMotion(); StopBucketMotion(); _CraneInAutoMode = false; }
	void DisableAll() { _dolly.DisableStepper(); _tower.DisableStepper(); _ropebarrel.Disable(); }

	void MoveDollyOutwards() { /*_dolly.MoveOut();*/ _dolly.MoveOutMM(100); }
	void MoveDollyInwards() { /*_dolly.MoveIn();*/ _dolly.MoveInMM(100); }
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

	int GetCandy1Measurement();
	int GetCandy2Measurement();
};

