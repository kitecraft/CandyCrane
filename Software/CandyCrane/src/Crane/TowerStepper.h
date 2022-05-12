#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "../Utilities/CraneStepper.h"

class TowerStepper : public CraneStepper
{
private:

public:
	TowerStepper();
	bool Init();
};

