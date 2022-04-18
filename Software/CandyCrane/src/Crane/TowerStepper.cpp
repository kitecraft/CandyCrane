#include "TowerStepper.h"

TowerStepper::TowerStepper()
{

}

bool TowerStepper::Init()
{
	_muxTowerMotion = portMUX_INITIALIZER_UNLOCKED;
	_towerMaxiumSteps = TowerStepsForDistance(ROPE_BARREL_MAXIMUM_DISTANCE);
	_stepper = new CheapStepper(TOWER_STEPPER_PIN_1, TOWER_STEPPER_PIN_2, TOWER_STEPPER_PIN_3, TOWER_STEPPER_PIN_4);
	_stepper->setRpm(7);
	return true;
}

bool TowerStepper::Calibrate()
{

	return true;
}

void TowerStepper::Process()
{
	if (IsTowerInMotion())
	{
		if (_errorCondition)
		{
			Serial.println("TowerStepper: Error condition exists.  Unable to move dolly.");
			_stepper->stop();
			SetTowerMotionStatus(false);
			return;
		}

		if (_stepper->getStepsLeft() != 0)
		{
			/*
			if (_stepper->getStep() < _ropeBarrelMaxiumSteps + 1)
			{
				_stepper->run();
			}
			else {
				Serial.println("TowerStepper: Bucket: Ending run due to a limit being hit");
				_stepper->stop();
				SetBucketMotionStatus(false);
				_errorCondition = true;
			}
			*/
			_stepper->run();
		}
		else {
			Serial.println("TowerStepper: Bucket Move complete.");
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
	Serial.printf("Tower: Current step  '%i' \n", _stepper->getStep());
	int val = _towerMaxiumSteps - _stepper->getStep();
	Serial.printf("Moving Tower Outwards '%i' steps\n", val);
	_stepper->newMoveCW(val);
	SetTowerMotionStatus(true);
}

void TowerStepper::MoveTowerInwards()
{
	if (_errorCondition) {
		Serial.println("Tower: Error condition exists.  Unable to move tower.");
		return;
	}
	Serial.println("Moveing Tower Inwards");
	_stepper->newMoveCCW(_stepper->getStep());  // moves x steps to 0 where is is the current step count
	SetTowerMotionStatus(true);
}

void TowerStepper::StopTower()
{
	Serial.println("Stop the Tower, I'm getting off now.");
	_stepper->stop();
}