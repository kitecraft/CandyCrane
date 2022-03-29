#include "CraneController.h"
#include "../ESPNow/EspNowManager.h"

void CraneController::StartUp()
{
	_muxBucketDistance = portMUX_INITIALIZER_UNLOCKED;
	_muxBucketMotion = portMUX_INITIALIZER_UNLOCKED;

	while(!_bucketConnected){
		WaitforBucketConnect();
	}
	if (!GetBucketDistance()) {
		Serial.println("Failed to get distance");
		// trigger general error condition
	}

	CloseBucket();


	Serial.printf("Bucket distance is: '%i'", _bucketDistance);

}

bool CraneController::WaitforBucketConnect()
{
	Serial.println("\nFinding bucket");
	_bucketHeartbeatsWaiting = 0;
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
		portENTER_CRITICAL(&_muxBucketDistance);
		uint32_t distance = _bucketDistance;
		portEXIT_CRITICAL(&_muxBucketDistance);
		
		if(distance != BUCKET_NO_DISTANCE)
		{
			return true;
		}
		delay(5);
	}
	return false;
}

bool CraneController::OpenBucket()
{
	return OpenCloseBucket(CC_OPEN_BUCKET);
}

bool CraneController::CloseBucket()
{
	return OpenCloseBucket(CC_CLOSE_BUCKET);
}

bool CraneController::OpenCloseBucket(CANDY_CRANE_COMMANDS openclose)
{
	_bucketMoveComplete = false;
	if (!SendEspNowCommand(openclose))
	{
		return false;
	}

	unsigned long endWait = millis() + BUCKET_MOVE_WAIT_TIME;
	while (millis() < endWait) {
		portENTER_CRITICAL(&_muxBucketMotion);
		bool state = _bucketMoveComplete;
		portEXIT_CRITICAL(&_muxBucketMotion);

		if (state)
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
	int counter = 0;
	while (true) {
		/*
		if (!_bucketConnected) {
			// trigger general error condition
			WaitforBucketConnect();
		}
		*/
		counter++;
		GetBucketDistance();
		Serial.printf("%i Bucket distance is: '%i'\n",counter, _bucketDistance);
		delay(500);

		if (_nextMove < millis())
		{
			if (_bucketState)
			{
				Serial.println("Closing the bucket, dear Liza, dear Liza");
				if (CloseBucket()) {
					Serial.println("Bucket has been closed");
				}
				else {
					Serial.println("Failed to close bucket");
				}
			}
			else {
				Serial.println("Opening the bucket, dear Liza, dear Liza");
				if (OpenBucket()) {
					Serial.println("Bucket has been opened");
				}
				else {
					Serial.println("Failed to open bucket");
				}
			}
			_bucketState = !_bucketState;
			_nextMove = millis() + 5000;
		}
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
				portENTER_CRITICAL(&_muxBucketDistance);
				_bucketDistance = currentMessage.value;
				portEXIT_CRITICAL(&_muxBucketDistance);
				break;
			case CC_BUCKET_MOVE_COMPLETE:
				portENTER_CRITICAL(&_muxBucketMotion);
				_bucketMoveComplete = true;
				portEXIT_CRITICAL(&_muxBucketMotion);
				break;
			default:
				Serial.print("Found an oddity in the queue!");
				Serial.print(currentMessage.command);
				Serial.print("  -  ");
				Serial.println(currentMessage.value);
				break;
			}
		}
		delay(1);
	}
}