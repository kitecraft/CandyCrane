#include "TowerStepper.h"

TowerStepper::TowerStepper()
{

}

bool TowerStepper::Init()
{
	_muxTowerMotion = portMUX_INITIALIZER_UNLOCKED;
	_stepper = new CheapStepper(TOWER_STEPPER_PIN_1, TOWER_STEPPER_PIN_2, TOWER_STEPPER_PIN_3, TOWER_STEPPER_PIN_4);
	_stepper->setTotalSteps(1024);
	_stepper->setRpm(2);
	_towerMaxiumSteps = TowerStepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	pinMode(TOWER_LIMIT_SWITCH_PIN, INPUT_PULLUP);
	return true;
}

bool TowerStepper::Calibrate()
{
	if (IsTowerLimitSwitchActive())
	{
		_stepper->moveCW(TowerStepsForDistance(10));
		if (IsTowerLimitSwitchActive())
		{
			Serial.println("Tower: Calibration fail 1:  The limit switch is still active");
			return false;
		}
	}

	while (!IsTowerLimitSwitchActive())
	{
		_stepper->stepCCW();
		vTaskDelay(1);
	}

	//_stepper->stop();
	delay(100);
	_stepper->moveCW(TowerStepsForDistance(5));
	_stepper->setCurrentPositionAsHome();

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
			_stepper->stop();
			SetTowerMotionStatus(false);
			return;
		}

		if (_stepper->getStepsLeft() != 0)
		{
			if (_stepper->getStep() < _towerMaxiumSteps + 1)
			{
				_stepper->run();
			}
			else {
				Serial.println("Tower: Bucket: Ending run due to a limit being hit");
				_stepper->stop();
				SetTowerMotionStatus(false);
				_errorCondition = true;
			}
			_stepper->run();
		}
		else {
			SetTowerMotionStatus(false);
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
	int val = _towerMaxiumSteps - _stepper->getStep();
	_stepper->newMoveCW(val);
	SetTowerMotionStatus(true);
}

void TowerStepper::MoveTowerInwards()
{
	if (_errorCondition) {
		Serial.println("Tower: Error condition exists.  Unable to move tower.");
		return;
	}
	_stepper->newMoveCCW(_stepper->getStep());  // moves x steps to 0 where is is the current step count
	SetTowerMotionStatus(true);
}

void TowerStepper::StopTower()
{
	_stepper->stop();
}

bool TowerStepper::IsTowerLimitSwitchActive()
{
	if (digitalRead(TOWER_LIMIT_SWITCH_PIN) == LOW) {
		return true;
	}
	return false;
}