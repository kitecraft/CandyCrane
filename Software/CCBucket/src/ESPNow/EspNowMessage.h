#pragma once
#include <Arduino.h>
#include "CandyCraneCommands.h"


typedef struct EspNowMessage {
    int command = CC_NONE;
    int distance = -1;
    int angle = -1;
    int speed = -1;

    EspNowMessage() {}
    EspNowMessage(const EspNowMessage& orig)
    {
        command = orig.command;
        distance = orig.distance;
        angle = orig.angle;
        speed = orig.speed;
    }
} EspNowMessage;
