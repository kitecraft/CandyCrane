#pragma once
#include <Arduino.h>
#include "../ESPNow/EspNowIncomingMessageQueue.h"

extern EspNowIncomingMessageQueue g_espNowMessageQueue;

class CraneController
{
private:

public:
	bool StartUp();
	void Run();
	
};

