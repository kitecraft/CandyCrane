#include "IncomeingMessageQueue.h"

IncomeingMessageQueue::IncomeingMessageQueue()
{
	ArduinoQueue<EspNowMessage> messageQueue(5);
	QueueMutex = portMUX_INITIALIZER_UNLOCKED;
}

void IncomeingMessageQueue::AddItemToQueue(EspNowMessage message)
{
	EspNowMessage qi(message);

	portENTER_CRITICAL(&QueueMutex);
	messageQueue.enqueue(qi);
	portEXIT_CRITICAL(&QueueMutex);
}

bool IncomeingMessageQueue::IsQueueEmpty()
{
	return messageQueue.isEmpty();
}

EspNowMessage IncomeingMessageQueue::GetNextItem()
{
	EspNowMessage ret;

	portENTER_CRITICAL(&QueueMutex);
	ret = messageQueue.dequeue();
	portEXIT_CRITICAL(&QueueMutex);

	return ret;
}