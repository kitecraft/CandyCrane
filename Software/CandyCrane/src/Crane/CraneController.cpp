#include "CraneController.h"

bool CraneController::StartUp()
{
	_muxBucketOpenClose = portMUX_INITIALIZER_UNLOCKED;
	
	_dolly.Init(DOLLY_STEPPER_PIN_1, DOLLY_STEPPER_PIN_2, DOLLY_STEPPER_PIN_3, DOLLY_STEPPER_PIN_4);
	_dolly.ConnectToLimitSwitch(DOLLY_LIMIT_SWITCH_PIN);
	_dolly.SetStepsPerMM(DOLLY_STEPS_PER_MM);
	_dolly.SetMaximumDistance(DOLLY_MAXIMUM_DISTANCE);
	_dolly.SetSpeed(DOLLY_SPEED_STEPS_SECOND);
	_dolly.SetAcceleration(DOLLY_ACCEL_STEPS_SECOND);

	
	_tower.Init(TOWER_STEPPER_PIN_1, TOWER_STEPPER_PIN_2, TOWER_STEPPER_PIN_3, TOWER_STEPPER_PIN_4);
	_tower.ConnectToLimitSwitch(TOWER_LIMIT_SWITCH_PIN);
	_tower.SetStepsPerMM(TOWER_STEPS_PER_MM);
	_tower.SetMaximumDistance(TOWER_MAXIMUM_DISTANCE);
	_tower.SetSpeed(TOWER_SPEED_STEPS_SECOND);
	_tower.SetAcceleration(TOWER_ACCEL_STEPS_SECOND);

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
	if (!WaitforBucketConnect()) {
		return false;
	}

	CloseBucket();
	OpenBucket();
	CloseBucket();

	return _ropebarrel.Calibrate();
}

bool CraneController::CalibrateAll()
{
	Serial.println("Calibrating bucket");
	CalibrateBucket();
	
	Serial.println("Calibrating dolly");
	if (!_dolly.Calibrate(DOLLY_CALIBRATION_SPEED))
	{
		return false;
	}

	Serial.println("Calibrating tower");
	if (!_tower.Calibrate(TOWER_CALIBRATION_SPEED))
	{
		return false;
	}

	_tower.DisableStepper();
	_dolly.DisableStepper();
	_ropebarrel.Disable();

	Serial.println("Ending Calibration");
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


bool CraneController::OpenCloseBucket(int moveToAngle, int moveingSpeed, bool async)
{
	_bucketOpenCloseComplete = false;

	EspNowMessage message;
	message.command = CC_MOVE_BUCKET;
	message.angle = moveToAngle;
	message.speed = moveingSpeed;
	SendMessage(message);

	if (async) {
		return true;
	}

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


void CraneController::Run()
{
	if (!_QueueHandlerRunning) {
		Serial.println("RunQueueHandler() must be running first!");
		return;
	}

	int counter = 0;
	while (true) {
		
		_dolly.Process();
		_ropebarrel.Process();
		_tower.Process();
		
		
		/*
		if (!_bucketConnected) {
			// trigger general error condition
			WaitforBucketConnect();
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
				_ropebarrel.SetBucketDistance(currentMessage.distance);
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