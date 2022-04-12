#pragma once
#include <Arduino.h>


typedef struct EspNowMessage {
    int command = -1;
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
    EspNowMessage(String csv) {
        int sep = csv.indexOf(",");
        command = csv.substring(0, sep).toInt();

        int last = sep + 1;
        sep = csv.indexOf(",", last);
        distance = csv.substring(last, sep).toInt();

        last = sep + 1;
        sep = csv.indexOf(",", last);
        angle = csv.substring(last, sep).toInt();
        
        last = sep + 1;
        speed = csv.substring(last).toInt();
    }
    String ToString() {
        String ret = static_cast<String>(command);
        ret += ",";
        ret += String(distance);
        ret += ",";
        ret += String(angle);
        ret += ",";
        ret += String(speed);
        return ret;
    }
} EspNowMessage;
