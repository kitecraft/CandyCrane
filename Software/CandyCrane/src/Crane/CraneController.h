#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../ESPNow/EspNowIncomingMessageQueue.h"


extern EspNowIncomingMessageQueue g_espNowMessageQueue;

class CraneController
{
private:
	bool _QueueHandlerRunning = false;
	bool _bucketConnected = false;
	uint32_t _bucketDistance = BUCKET_NO_DISTANCE;

	uint8_t _bucketHeartbeatsWaiting = 0;
	unsigned long _nextHeartbeat = 0;

	bool WaitforBucketConnect();
	bool GetBucketDistance();

	bool SendBucketHeartbeat();
public:
	void StartUp();
	void Run();
	void RunQueueHandler();
};

