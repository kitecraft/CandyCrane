#include "CraneController.h"
#include "../ESPNow/EspNowManager.h"

void CraneController::StartUp()
{
	while(!_bucketConnected){
		WaitforBucketConnect();
	}
	if (!GetBucketDistance()) {
		Serial.println("Failed to get distance");
		// trigger general error condition
	}
	Serial.printf("Bucket distance is: '%i'", _bucketDistance);

}

bool CraneController::WaitforBucketConnect()
{
	Serial.println("Finding bucket");
	while (!_bucketConnected)
	{
		if (SendBucketHeartbeat())
		{
			Serial.print(".");
		}
		if (_bucketHeartbeatsWaiting >= BUCKET_LOST_PONG_COUNT) {
			return false;
		}
		delay(10);
	}
	Serial.println("\nBucket has been found");
	return true;
}

bool CraneController::GetBucketDistance()
{
	_bucketDistance = BUCKET_NO_DISTANCE;
	if (!SendEspNowCommand(CC_GET_DISTANCE))
	{
		return false;
	}
	unsigned long endWait = millis() + BUCKET_COMMAND_WAIT_TIME;
	while (millis() < endWait) {
		if(_bucketDistance != BUCKET_NO_DISTANCE)
		{
			return true;
		}
		delay(5);
	}
	return false;
}

bool CraneController::SendBucketHeartbeat()
{
	if (_nextHeartbeat < millis())
	{
		if (_bucketHeartbeatsWaiting > BUCKET_HEARBEATS_MAX_MISSED) {
			_bucketConnected = false;
		}
		bool ret = SendEspNowCommand(CC_PING);
		_bucketHeartbeatsWaiting++;
		_nextHeartbeat = millis() + BUCKET_HEARTBEAT_INTERVAL;
		return ret;
	}
	return false;
}

void CraneController::Run()
{
	if (!_QueueHandlerRunning) {
		Serial.println("RunQueueHandler() must be running first!");
		return;
	}

	//StartUp();
	
	while (true) {
		/*
		if (!_bucketConnected) {
			// trigger general error condition
			WaitforBucketConnect();
		}
		*/

		delay(1);
	}
}

void CraneController::RunQueueHandler()
{
	_QueueHandlerRunning = true;
	while (true) {
		if (!g_espNowMessageQueue.IsQueueEmpty()) {
			EspNowMessage currentMessage = g_espNowMessageQueue.GetNextItem();
			switch (currentMessage.command) {
			case CC_PONG:
				_bucketConnected = true;
				_bucketHeartbeatsWaiting = 0;
				break;
			case CC_BUCKET_DISTANCE:
				_bucketDistance = currentMessage.value;
				break;
			default:
				break;
			}
		}
		delay(2);
	}
}