#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../ESPNow/EspNowMessageQueue.h"
#include "../ESPNow/EspNowMessage.h"
#include "../ESPNow/EspNowManager.h"
#include "DollyStepper.h"
#include <CheapStepper.h>

extern EspNowMessageQueue g_espNowMessageQueue;

class CraneController
{
private:
	bool _QueueHandlerRunning = false;
	bool _bucketConnected = false;
	uint32_t _bucketDistance = BUCKET_NO_DISTANCE;
	portMUX_TYPE _muxBucketDistance;
	bool _bucketOpenCloseComplete = false;
	portMUX_TYPE _muxBucketOpenClose;

	uint8_t _bucketHeartbeatsWaiting = 0;
	unsigned long _nextHeartbeat = 0;

	bool WaitforBucketConnect();
	bool GetBucketDistance();
	bool OpenBucket();
	bool CloseBucket();
	bool OpenCloseBucket(int moveToAngle, int moveSpeed);
	bool SendBucketHeartbeat();

	//CheapStepper *_dollyStepper;
	CheapStepper *_ropeBarrelStepper;
	DollyStepper _dolly;


	bool CalibrateBucket();
	bool CalibrateTower();
	bool CalibrateAll();

	/* stuff for testing, remove later or else....*/
	unsigned long _nextMove = 0;
	bool _bucketState = false;

public:
	void StartUp();
	void Run();
	void RunQueueHandler();


	void MoveDollyOutwards();
	void MoveDollyInwards();
	void StopDolly();
	bool IsDollyInMotion();
	void RecalibrateDolly();
};

