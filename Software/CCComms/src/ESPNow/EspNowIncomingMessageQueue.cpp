#include "EspNowIncomingMessageQueue.h"

EspNowIncomingMessageQueue::EspNowIncomingMessageQueue()
{
	ArduinoQueue<EspNowMessage> messageQueue(3);
}

void EspNowIncomingMessageQueue::AddItemToQueue(EspNowMessage message)
{
	messageQueue.enqueue(EspNowMessage(message));
}

bool EspNowIncomingMessageQueue::IsQueueEmpty()
{
	return messageQueue.isEmpty();
}

EspNowMessage EspNowIncomingMessageQueue::GetNextItem()
{
	return messageQueue.dequeue();
}