#pragma once
#include <Arduino.h>
//#include <CheapStepper.h>
#include "../SmallFlexyStepper/SmallFlexyStepper.h"
#include "../CC_Config.h"
class TowerStepper
{
private:
	//CheapStepper* _stepper;
	SmallFlexyStepper _stepper;
	bool _isTowerInMotion = false;
	portMUX_TYPE _muxTowerMotion;
	int _towerMaxiumSteps = 0;
	void SetTowerMotionStatus(bool status);
	int TowerStepsForDistance(double distance) { return int(distance * (double)TOWER_STEPS_PER_MM); }
	bool IsTowerLimitSwitchActive();

	bool _errorCondition = false;

public:
	TowerStepper();
	bool Init();
	bool Calibrate();

	void Process();
	void MoveTowerOutwards();
	void MoveTowerInwards();
	void MoveTowerTo(int mm);
	void StopTower();
	bool IsTowerInMotion();
};

