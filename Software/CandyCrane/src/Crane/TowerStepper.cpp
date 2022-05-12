#include "TowerStepper.h"

TowerStepper::TowerStepper()
{

}

bool TowerStepper::Init()
{
	_muxTowerMotion = portMUX_INITIALIZER_UNLOCKED;
	//_stepper = new CheapStepper(TOWER_STEPPER_PIN_1, TOWER_STEPPER_PIN_2, TOWER_STEPPER_PIN_3, TOWER_STEPPER_PIN_4);
	//_stepper->setTotalSteps(1024);
	//_stepper->setRpm(2);

	_stepper.connectToPins(TOWER_STEPPER_PIN_1, TOWER_STEPPER_PIN_2, TOWER_STEPPER_PIN_3, TOWER_STEPPER_PIN_4);
	_stepper.setSpeedInStepsPerSecond(TOWER_SPEED_STEPS_SECOND);
	_stepper.setAccelerationInStepsPerSecondPerSecond(TOWER_ACCEL_STEPS_SECOND);
	_stepper.setStepsPerMillimeter(TOWER_STEPS_PER_MM);

	_towerMaxiumSteps = TowerStepsForDistance(TOWER_MAXIMUM_DISTANCE);
	Serial.printf("_towerMaxiumStepsp: %i\n", _towerMaxiumSteps);
	pinMode(TOWER_LIMIT_SWITCH_PIN, INPUT_PULLUP);
	return true;
}

bool TowerStepper::Calibrate()
{
	_stepper.moveToHomeInSteps(-1, 100, _towerMaxiumSteps + 20, TOWER_LIMIT_SWITCH_PIN);
	/*
	if (IsTowerLimitSwitchActive())
	{
		//_stepper->moveCW(TowerStepsForDistance(10));
		_stepper.moveRelativeInSteps(TowerStepsForDistance(10));
		if (IsTowerLimitSwitchActive())
		{
			Serial.println("Tower: Calibration fail 1:  The limit switch is still active");
			return false;
		}
	}

	while (!IsTowerLimitSwitchActive())
	{
		//_stepper->stepCCW();
		_stepper.moveRelativeInSteps(-1);
		vTaskDelay(1);
	}

	//_stepper->stop();
	delay(100);
	//_stepper->moveCW(TowerStepsForDistance(3));
	//_stepper->setCurrentPositionAsHome();
	_stepper.moveRelativeInSteps(TowerStepsForDistance(3));
	*/

	_stepper.setCurrentPositionInSteps(0);
	Serial.printf("Tower HOME step: %i\n", _stepper.getCurrentPositionInSteps());
	_errorCondition = false;
	return true;
}

void TowerStepper::Process()
{
	if (IsTowerInMotion())
	{
		if (_errorCondition)
		{
			Serial.println("Tower: Error condition exists.  Unable to move dolly.");
			//_stepper->stop();
			_stepper.deadStop();
			SetTowerMotionStatus(false);
			return;
		}

		//if (_stepper->getStepsLeft() != 0)
		if (!_stepper.motionComplete())
		{
			//if (!IsTowerLimitSwitchActive() && (_stepper->getStep() < _towerMaxiumSteps + 1))
			if (!IsTowerLimitSwitchActive() && (_stepper.getCurrentPositionInSteps() <= _towerMaxiumSteps + 1))
			{
				//_stepper->run();
				_stepper.processMovement();
			}
			else {
				Serial.println("Tower: Bucket: Ending run due to a limit being hit");
				//_stepper->stop();
				_stepper.deadStop();
				SetTowerMotionStatus(false);
				_errorCondition = true;
			}
			//_stepper->run();
		}
		else {
			SetTowerMotionStatus(false);
			Serial.printf("Tower step: %i\n", _stepper.getCurrentPositionInSteps());
		}
	}
}


void TowerStepper::SetTowerMotionStatus(bool status)
{
	portENTER_CRITICAL(&_muxTowerMotion);
	_isTowerInMotion = status;
	portEXIT_CRITICAL(&_muxTowerMotion);
}

bool TowerStepper::IsTowerInMotion()
{
	portENTER_CRITICAL(&_muxTowerMotion);
	bool ret = _isTowerInMotion;
	portEXIT_CRITICAL(&_muxTowerMotion);

	return ret;
}

void TowerStepper::MoveTowerOutwards()
{
	if (_errorCondition) {
		Serial.println("Tower: Error condition exists.  Unable to move tower.");
		return;
	}
	//int val = _towerMaxiumSteps - _stepper->getStep();
	//_stepper->newMoveCW(val);
	_stepper.setTargetPositionInMillimeters(TOWER_MAXIMUM_DISTANCE);
	//_stepper.setTargetPositionInMillimeters(_stepper.getCurrentPositionInMillimeters() + 10.0);
	SetTowerMotionStatus(true);
}

void TowerStepper::MoveTowerInwards()
{
	if (_errorCondition) {
		Serial.println("Tower: Error condition exists.  Unable to move tower.");
		return;
	}
	//_stepper->newMoveCCW(_stepper->getStep());  // moves x steps to 0 where is is the current step count
	_stepper.setTargetPositionInMillimeters(0);
	//_stepper.setTargetPositionInMillimeters(_stepper.getCurrentPositionInMillimeters() - 10.0);
	SetTowerMotionStatus(true);
}

void TowerStepper::MoveTowerTo(int mm)
{
	if (_errorCondition) {
		Serial.println("Tower: Error condition exists.  Unable to move tower.");
		return;
	}

	//long stepToMoveTo = TowerStepsForDistance(mm);
	_stepper.setTargetPositionInMillimeters(mm);
	/*
	int stepToMoveTo = TowerStepsForDistance(mm);
	int stepsToMove = stepToMoveTo - _stepper->getStep();
	if (stepsToMove > 0)
	{
		_stepper->newMoveCW(stepsToMove);
	}
	else {
		_stepper->newMoveCCW(stepsToMove * -1);
	}
	*/
	SetTowerMotionStatus(true);
}

void TowerStepper::StopTower()
{
	//_stepper->stop();
	_stepper.deadStop();
}

bool TowerStepper::IsTowerLimitSwitchActive()
{
	if (digitalRead(TOWER_LIMIT_SWITCH_PIN) == LOW) {
		return true;
	}
	return false;
}