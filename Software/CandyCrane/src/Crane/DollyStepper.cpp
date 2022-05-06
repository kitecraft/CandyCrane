#include "DollyStepper.h"

DollyStepper::DollyStepper()
{

}

bool DollyStepper::Init()
{
	_muxDollyMotion = portMUX_INITIALIZER_UNLOCKED;

	_stepper = new CheapStepper(DOLLY_STEPPER_PIN_1, DOLLY_STEPPER_PIN_2, DOLLY_STEPPER_PIN_3, DOLLY_STEPPER_PIN_4);
	_stepper->setRpm(12);
	_dollyMaxiumSteps = DollyStepsForDistance(DOLLY_MAXIMUM_DISTANCE);
	pinMode(DOLLY_LIMIT_SWITCH_PIN, INPUT_PULLUP);
}


bool DollyStepper::Calibrate()
{
	Serial.println("Calibrating the Dolly");
	if (IsDollyLimitSwitchActive())
	{
		_stepper->newMoveCW(DollyStepsForDistance(10));
		while (_stepper->getStepsLeft() > 0)
		{
			_stepper->run();
			vTaskDelay(1);
		}
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

	_stepper->stop();
	_stepper->moveCW(DollyStepsForDistance(5));
	_stepper->setCurrentPositionAsHome();


	Serial.println("Dolly is Home");
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
	Serial.printf("Dolly: Current step  '%i' \n", _stepper->getStep());
	int val = _dollyMaxiumSteps - _stepper->getStep();
	Serial.printf("Moving Dolly Outwards '%i' steps\n", val);
	_stepper->newMoveCW(val);
	SetDollyMotionStatus(true);
}

void DollyStepper::MoveDollyInwards()
{
	if (_errorCondition) {
		Serial.println("Dolly: Error condition exists.  Unable to move dolly.");
		return;
	}
	Serial.println("Moveing Dolly Inwards");
	//_stepper->newMoveCCW(_stepper->getStep());  // moves x steps to 0 where is is the current step count
	_stepper->newMoveCCW(1000);  // moves x steps to 0 where is is the current step count
	SetDollyMotionStatus(true);
}

void DollyStepper::StopDolly()
{
	Serial.println("Stop the Dolly, I'm getting off now.");
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
			Serial.println("Dolly Move complete.");
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