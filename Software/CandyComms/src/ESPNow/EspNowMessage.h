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

static EspNowMessage EspNowMessageFromCsvString(String csv)
{
    EspNowMessage ret;
    int sep = csv.indexOf(",");
    ret.command = csv.substring(0, sep).toInt();

    int last = sep + 1;
    sep = csv.indexOf(",", last);
    ret.distance = csv.substring(last, sep).toInt();

    last = sep + 1;
    sep = csv.indexOf(",", last);
    ret.angle = csv.substring(last, sep).toInt();

    last = sep + 1;
    ret.speed = csv.substring(last).toInt();

    return ret;
}

static String EspNowMessageToCsvString(EspNowMessage msg)
{
    String ret = static_cast<String>(msg.command);
    ret += ",";
    ret += String(msg.distance);
    ret += ",";
    ret += String(msg.angle);
    ret += ",";
    ret += String(msg.speed);
    return ret;
}