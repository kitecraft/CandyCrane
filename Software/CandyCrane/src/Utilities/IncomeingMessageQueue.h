#pragma once
#include <Arduino.h>
#include <ArduinoQueue.h>
#include "../ESPNow/EspNowMessage.h"

class IncomeingMessageQueue
{
private:
	ArduinoQueue<EspNowMessage> messageQueue;
	portMUX_TYPE QueueMutex;

public:
	IncomeingMessageQueue();
	void AddItemToQueue(EspNowMessage message);
	bool IsQueueEmpty();
	EspNowMessage GetNextItem();
};

