#include "DollyStepper.h"

DollyStepper::DollyStepper()
{

}

bool DollyStepper::Init()
{
	_muxDollyMotion = portMUX_INITIALIZER_UNLOCKED;

	_stepper = new CheapStepper(DOLLY_STEPPER_PIN_1, DOLLY_STEPPER_PIN_2, DOLLY_STEPPER_PIN_3, DOLLY_STEPPER_PIN_4);
	_stepper->setRpm(20);
	_dollyMaxiumSteps = DollyStepsForDistance(DOLLY_MAXIMUM_DISTANCE);
	pinMode(DOLLY_LIMIT_SWITCH_PIN, INPUT_PULLUP);
}


bool DollyStepper::Calibrate()
{
	if (IsDollyLimitSwitchActive())
	{
		_stepper->moveCW(DollyStepsForDistance(10));
		if (IsDollyLimitSwitchActive())
		{
			Serial.println("Dolly: Calibration fail 1:  The limit switch is still active");
			return false;
		}
	}


	while (!IsDollyLimitSwitchActive())
	{
		_stepper->stepCCW();
		vTaskDelay(1);
	}

	delay(100);
	_stepper->moveCW(DollyStepsForDistance(5));
	_stepper->setCurrentPositionAsHome();

	_errorCondition = false;
	return true;
}

void DollyStepper::SetDollyMotionStatus(bool status)
{
	portENTER_CRITICAL(&_muxDollyMotion);
	_isDollyInMotion = status;
	portEXIT_CRITICAL(&_muxDollyMotion);
}

bool DollyStepper::IsDollyInMotion()
{
	portENTER_CRITICAL(&_muxDollyMotion);
	bool ret = _isDollyInMotion;
	portEXIT_CRITICAL(&_muxDollyMotion);

	return ret;
}


void DollyStepper::MoveDollyOutwards()
{
	if (_errorCondition) {
		Serial.println("Dolly: Error condition exists.  Unable to move dolly.");
		return;
	}
	int val = _dollyMaxiumSteps - _stepper->getStep();
	_stepper->newMoveCW(val);
	SetDollyMotionStatus(true);
}

void DollyStepper::MoveDollyInwards()
{
	if (_errorCondition) {
		Serial.println("Dolly: Error condition exists.  Unable to move dolly.");
		return;
	}
	_stepper->newMoveCCW(_stepper->getStep());  // moves x steps to 0 where is is the current step count
	SetDollyMotionStatus(true);
}

void DollyStepper::StopDolly()
{
	_stepper->stop();
}

void DollyStepper::Process()
{
	if (IsDollyInMotion())
	{
		if (_errorCondition)
		{
			Serial.println("Dolly: Error condition exists.  Unable to move dolly.");
			_stepper->stop();
			SetDollyMotionStatus(false);
			return;
		}

		if (_stepper->getStepsLeft() != 0)
		{
			if (!IsDollyLimitSwitchActive() && (_stepper->getStep() < _dollyMaxiumSteps + 1))
			{
				_stepper->run();
			}
			else {
				Serial.println("Dolly: Ending run due to a limit being hit");
				_stepper->stop();
				SetDollyMotionStatus(false);
				_errorCondition = true;
			}

		}
		else {
			SetDollyMotionStatus(false);
		}
	}
}


bool DollyStepper::IsDollyLimitSwitchActive()
{
	if (digitalRead(DOLLY_LIMIT_SWITCH_PIN) == LOW) {
		return true;
	}
	return false;
}