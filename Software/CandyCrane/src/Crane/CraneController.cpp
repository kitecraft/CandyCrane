#include "CraneController.h"

bool CraneController::StartUp()
{
	_muxBucketDistance = portMUX_INITIALIZER_UNLOCKED;
	_muxBucketOpenClose = portMUX_INITIALIZER_UNLOCKED;

	_dolly.Init();
	_tower.Init();
	_ropebarrel.Init();
	
	if (!CalibrateAll())
	{
		Serial.println("CraneController::StartUp() - Failed to calibrate");
		return false;
	}
	return true;
}

bool CraneController::CalibrateBucket()
{
	Serial.println("\n\n--Bucket Calibartion\n");
	while (!_bucketConnected) {
		WaitforBucketConnect();
	}

	CloseBucket();
	OpenBucket();
	CloseBucket();

	if (!GetBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance.");
		return false;
	}
	//Serial.printf("Bucket distance is: '%i'\n", _bucketDistance);

	int distanceToMove = _bucketDistance - BUCKET_HOME_DISTANCE;
	Serial.printf("Moving bucket %i mm\n", distanceToMove);
	if (distanceToMove < 0) {
		_ropebarrel.DropBucket(distanceToMove * -1);
	}
	else {
		_ropebarrel.RaiseBucket(distanceToMove);
	}

	while (_ropebarrel.IsBucketInMotion())
	{
		_ropebarrel.Process();
		vTaskDelay(1);
	}

	delay(1000);

	if (!GetBucketDistance())
	{
		Serial.println("CalibrateBucket(): Failed to get bucket distance After Move.");
		return false;
	}
	//Serial.printf("Bucket distance is: '%i'\n\n\n--------\n", _bucketDistance);
	_ropebarrel.SetBucketHomeAsCurrent();

	return true;
}

bool CraneController::CalibrateAll()
{
	if (!CalibrateBucket())
	{
		return false;
	}

	if (!_dolly.Calibrate())
	{
		return false;
	}

	if (!_tower.Calibrate())
	{
		return false;
	}

	return true;
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
		vTaskDelay(10);
	}
	Serial.println("\nBucket has been found");
	return true;
}

bool CraneController::GetBucketDistance()
{
	_bucketDistance = BUCKET_NO_DISTANCE;
	SendCommand(CC_GET_DISTANCE);

	// wait for distance to be updated
	unsigned long endWait = millis() + BUCKET_COMMAND_WAIT_TIME;
	while (millis() < endWait) {
		portENTER_CRITICAL(&_muxBucketDistance);
		uint32_t distance = _bucketDistance;
		portEXIT_CRITICAL(&_muxBucketDistance);
		
		if(distance != BUCKET_NO_DISTANCE)
		{
			Serial.printf("Bucket distance is: '%i'\n", distance);
			return true;
		}
		vTaskDelay(5);
	}
	return false;
}

bool CraneController::OpenBucket()
{
	return OpenCloseBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED);
}

bool CraneController::CloseBucket()
{
	return OpenCloseBucket(DEFAULT_BUCKET_CLOSED_ANGLE, DEFAULT_BUCKET_CLOSE_SPEED);
}

bool CraneController::OpenCloseBucket(int moveToAngle, int moveingSpeed)
{
	_bucketOpenCloseComplete = false;

	EspNowMessage message;
	message.command = CC_MOVE_BUCKET;
	message.angle = moveToAngle;
	message.speed = moveingSpeed;
	SendMessage(message);

	unsigned long endWait = millis() + BUCKET_MOVE_WAIT_TIME;
	while (millis() < endWait) {
		portENTER_CRITICAL(&_muxBucketOpenClose);
		bool state = _bucketOpenCloseComplete;
		portEXIT_CRITICAL(&_muxBucketOpenClose);

		if (state)
		{
			return true;
		}
		vTaskDelay(5);
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
		SendCommand(CC_PING);
		_bucketHeartbeatsWaiting++;
		_nextHeartbeat = millis() + BUCKET_HEARTBEAT_INTERVAL;
		return true;
	}
	return false;
}

void CraneController::MoveDollyOutwards()
{
	_dolly.MoveDollyOutwards();
}

void CraneController::MoveDollyInwards()
{
	_dolly.MoveDollyInwards();
}

void CraneController::StopDolly()
{
	_dolly.StopDolly();
}

bool CraneController::IsDollyInMotion()
{
	return _dolly.IsDollyInMotion();
}

void CraneController::MoveBucketDownwards()
{
	_ropebarrel.DropBucket();
}

void CraneController::MoveBucketUpwards()
{
	_ropebarrel.RaiseBucket();
}

void CraneController::StopBucketMotion()
{
	_ropebarrel.StopBucket();
}

void CraneController::MoveTowerOutwards()
{
	_tower.MoveTowerOutwards();
}

void CraneController::MoveTowerInwards()
{
	_tower.MoveTowerInwards();
}

void CraneController::StopTowerMotion()
{
	_tower.StopTower();
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
		

		//GetBucketDistance();
		//Serial.printf("%i Bucket distance is: '%i'\n", counter, _bucketDistance);
		//vTaskDelay(1000);

		
		_dolly.Process();
		_ropebarrel.Process();
		_tower.Process();
		
		
		/*
		if (!_bucketConnected) {
			// trigger general error condition
			WaitforBucketConnect();
		}
		*/

		/*
		counter++;
		
		if (_nextMove < millis())
		{
			if (_bucketState)
			{
				GetBucketDistance();
				Serial.printf("%i Bucket distance is: '%i'\n", counter, _bucketDistance);

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
		*/
		vTaskDelay(1);
	}
}

void CraneController::RunQueueHandler()
{
	_QueueHandlerRunning = true;
	while (true) {
		if (!g_incomeingMessageQueue.IsQueueEmpty()) {
			EspNowMessage currentMessage = g_incomeingMessageQueue.GetNextItem();
			switch (currentMessage.command) {
			case CC_PONG:
				_bucketConnected = true;
				_bucketHeartbeatsWaiting = 0;
				break;
			case CC_BUCKET_DISTANCE:
				portENTER_CRITICAL(&_muxBucketDistance);
				_bucketDistance = currentMessage.distance;
				portEXIT_CRITICAL(&_muxBucketDistance);
				break;
			case CC_BUCKET_MOVE_COMPLETE:
				portENTER_CRITICAL(&_muxBucketOpenClose);
				_bucketOpenCloseComplete = true;
				portEXIT_CRITICAL(&_muxBucketOpenClose);
				break;
			default:
				Serial.print("Found an oddity in the queue!");
				Serial.print(currentMessage.command);
				Serial.print("  -  Angle: ");
				Serial.print(currentMessage.angle);
				Serial.print("  -  Speed: ");
				Serial.print(currentMessage.speed);
				Serial.print("  -  Distance: ");
				Serial.print(currentMessage.distance);
				break;
			}
		}
		vTaskDelay(1);
	}
}