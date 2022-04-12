#pragma once
#include <Arduino.h>
#include <ArduinoQueue.h>
#include "EspNowMessage.h"

class EspNowMessageQueue
{
private:
	ArduinoQueue<EspNowMessage> messageQueue;
	portMUX_TYPE QueueMutex;

public:
	EspNowMessageQueue();
	void AddItemToQueue(EspNowMessage message);
	bool IsQueueEmpty();
	EspNowMessage GetNextItem();
};

