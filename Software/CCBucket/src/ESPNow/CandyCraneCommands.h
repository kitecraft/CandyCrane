#pragma once
#include <Arduino.h>

enum CANDY_CRANE_COMMANDS {
    CC_NONE = -1,
    CC_PING,
    CC_PONG,
    CC_MOVE_BUCKET,
    CC_BUCKET_MOVE_COMPLETE,
    CC_GET_DISTANCE,
    CC_BUCKET_DISTANCE,

    CC_ACK_COMMAND,
};