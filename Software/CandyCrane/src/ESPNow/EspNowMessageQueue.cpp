#include "EspNowMessageQueue.h"

EspNowMessageQueue::EspNowMessageQueue()
{
	ArduinoQueue<EspNowMessage> messageQueue(5);
	QueueMutex = portMUX_INITIALIZER_UNLOCKED;
}

void EspNowMessageQueue::AddItemToQueue(EspNowMessage message)
{
	EspNowMessage qi(message);

	portENTER_CRITICAL(&QueueMutex);
	messageQueue.enqueue(qi);
	portEXIT_CRITICAL(&QueueMutex);
}

bool EspNowMessageQueue::IsQueueEmpty()
{
	return messageQueue.isEmpty();
}

EspNowMessage EspNowMessageQueue::GetNextItem()
{
	EspNowMessage ret;

	portENTER_CRITICAL(&QueueMutex);
	ret = messageQueue.dequeue();
	portEXIT_CRITICAL(&QueueMutex);

	return ret;
}