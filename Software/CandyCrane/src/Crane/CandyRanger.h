#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include <ServoEasing.h>
#include <VL53L0X.h>

enum CANDY_ID {
	CANDY_A = 0,
	CANDY_B
};

class CandyRanger
{
private:
	ServoEasing _servo;
	VL53L0X _ranger;

	int GetDistanceForCandy(CANDY_ID candyId);
	bool InitRanger();
	bool InitServo();
public:
	bool Init();
	int GetDistanceForCandyA();
	int GetDistanceForCandyB();
};

