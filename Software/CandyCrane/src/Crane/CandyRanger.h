#pragma once
#include "../CC_Config.h"
#include <ServoEasing.h>



class CandyRanger
{
private:
	ServoEasing *_servo = nullptr;
public:
	bool Init();
};

