#pragma once
#include <Arduino.h>
#include <CheapStepper.h>
#include "../CC_Config.h"
class TowerStepper
{
private:
	CheapStepper* _stepper;
	bool _isTowerInMotion = false;
	portMUX_TYPE _muxTowerMotion;
	int _towerMaxiumSteps = 0;
	void SetTowerMotionStatus(bool status);
	int TowerStepsForDistance(double distance) { return int(distance * (double)TOWER_STEPS_PER_MM); }

	bool _errorCondition = false;

public:
	TowerStepper();
	bool Init();
	bool Calibrate();

	void Process();
	void MoveTowerOutwards();
	void MoveTowerInwards();
	void StopTower();
	bool IsTowerInMotion();
};

