#include "CraneController.h"
#include <Wire.h>
#include "CandyCraneCommands.h"

static void IRAM_ATTR tcpServerThread(void* pvParameters)
{
	CraneController* controller = (CraneController*)pvParameters;

	//Run the server.
	controller->RunTCPServer();  // this blocks until it's stopped

	//All good things...
	vTaskDelete(_tcpServerThreadHandle);
}

bool CraneController::StartUp()
{
	_muxBucketOpenClose = portMUX_INITIALIZER_UNLOCKED;

	Wire.begin(4, 5, 1000000);
	_stepperController.begin(Wire, MCP23017_DEFAULT_ADDR);
	
	_dolly.Init(_stepperController.getStepper(MTS_STEPPER_1)); _dolly.ConnectToLimitSwitch(DOLLY_LIMIT_SWITCH_PIN);
	_dolly.SetStepsPerMM(DOLLY_STEPS_PER_MM);
	_dolly.SetMaximumDistance(DOLLY_MAXIMUM_DISTANCE);
	_dolly.SetSpeed(DOLLY_SPEED_STEPS_SECOND);
	_dolly.SetAcceleration(DOLLY_ACCEL_STEPS_SECOND);

	_tower.Init(_stepperController.getStepper(MTS_STEPPER_2));
	_tower.ConnectToLimitSwitch(TOWER_LIMIT_SWITCH_PIN);
	_tower.SetStepsPerMM(TOWER_STEPS_PER_MM);
	_tower.SetMaximumDistance(TOWER_MAXIMUM_DISTANCE);
	_tower.SetSpeed(TOWER_SPEED_STEPS_SECOND);
	_tower.SetAcceleration(TOWER_ACCEL_STEPS_SECOND);

	_ropebarrel.Init(_stepperController.getStepper(MTS_STEPPER_3));
	_ropebarrel.setCallback([this]() { RequestDistance(); });

	_candyRanger.Init();

	xTaskCreate(
		tcpServerThread,
		"TCPServer",
		STACK_SIZE * 2,
		this,
		tskIDLE_PRIORITY + 8,
		&_tcpServerThreadHandle);
	
	//CalibrateBucket();

	return true;

	/*
	if (!CalibrateAll())
	{
		Serial.println("CraneController::StartUp() - Failed to calibrate");
		return false;
	}
	*/

}

bool CraneController::CalibrateBucket()
{
	Serial.println("\n\n--Bucket Calibartion\n");

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


void CraneController::WaitforBucketConnect()
{
	while (!_bucketConnected) {
		WiFiClient client = _tcpServer->available();
		if (client) {
			bool waitForHello = true;
			while (client.connected() && waitForHello) {
				if (client.available()) {
					String data;
					while (client.available()) {
						data = client.readStringUntil('\n');
					}
					data.trim();
					if (data == "") {
						Serial.println("No data received");
					}
					else {
						Serial.print("recieved from client: ");
						Serial.println(data);
						_bucketClient = client;
						_bucketConnected = true;
						waitForHello = false;
						return;
					}
				}
			}
		}
	}
}


bool CraneController::OpenCloseBucket(int moveToAngle, int moveingSpeed, bool async)
{
	_bucketOpenCloseComplete = false;
	SendMessageToBucket(CC_MOVE_BUCKET, 0, moveToAngle, moveingSpeed);

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

void CraneController::Run()
{
	bool bucketConnectionPreviousState = false;
	while (true) {
		if (_bucketConnected && !bucketConnectionPreviousState) {
			Serial.println("Bucket has connected, starting calibration");
			CalibrateAll();
			bucketConnectionPreviousState = _bucketConnected;
		}
		else if(!_bucketConnected) {
			bucketConnectionPreviousState = false;
		}

		if (_bucketConnected) {
			_dolly.Process();
			_ropebarrel.Process();
			_tower.Process();
		}
		vTaskDelay(1);
	}
}

void CraneController::RunTCPServer()
{
	_bucketConnected = false;
	Serial.println("Starting TCP Server");
	_tcpServer = new WiFiServer(8080, 1);
	_tcpServer->begin();
	Serial.println("Started...");

	while (true) {
		if (!_bucketConnected) {
			WaitforBucketConnect();
		}
		else {
			if (millis() - _lastPing > 5000) {
				Ping();
			}
			if (_bucketConnected && _bucketClient.available()) {
				String message = _bucketClient.readStringUntil('\n');
				ParseMessage(message);
			}
		}
		vTaskDelay(50);
	}
}

void CraneController::ParseMessage(String message)
{
	Serial.print("Got: '");
	Serial.print(message);
	Serial.println("'");

	int command;
	int distance;
	int angle;
	int speed;
	if (4 != sscanf(message.c_str(), "%d,%d,%d,%d", &command, &distance, &angle, &speed)) {
		return;
	}
	switch ((CANDY_CRANE_COMMANDS)command) {
	case CC_PONG:
		//Serial.println("Got Pong");
		_pinged = false;
		break;
	case CC_BUCKET_DISTANCE:
		Serial.printf("Setting the distance to: %d\n", distance);
		_ropebarrel.SetBucketDistance(distance);
		break;
	case CC_BUCKET_MOVE_COMPLETE:
		portENTER_CRITICAL(&_muxBucketOpenClose);
		_bucketOpenCloseComplete = true;
		portEXIT_CRITICAL(&_muxBucketOpenClose);
		break;
	default:
		Serial.print("Found an oddity in the queue!");
		Serial.print(command);
		Serial.print("  -  Angle: ");
		Serial.print(angle);
		Serial.print("  -  Speed: ");
		Serial.print(speed);
		Serial.print("  -  Distance: ");
		Serial.print(distance);
		break;
	};
}

void CraneController::Ping()
{
	if (_pinged) {
		_bucketClient.stop();
		_bucketConnected = false;
		_pinged = false;
		Serial.println("Lost connection to bucket");
		return;
	}

	_lastPing = millis();
	_pinged = true;
	SendMessageToBucket(CC_PING);
}

void CraneController::SendMessageToBucket(CANDY_CRANE_COMMANDS command, int distance, int angle, int speed)
{
	String message = String(command) + "," + String(distance) + "," + String(angle) + "," + String(speed);
	Serial.print("Sending: ");
	Serial.println(message);
	_bucketClient.println(message);
}