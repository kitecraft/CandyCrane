#pragma once
#include <Arduino.h>
#include "CandyCraneCommands.h"

class QueueItem
{
public:
	CANDY_CRANE_COMMANDS command = CC_NONE;
	int val = -1;
	QueueItem() {}

	QueueItem(CANDY_CRANE_COMMANDS inCommand, int inVal = -1) {
		command = inCommand;
		val = inVal;
	}

	QueueItem(const QueueItem& orig) {
		command = orig.command;
		val = orig.val;
	}
};