/*
 Name:		CCBucket.ino
 Created:	3/26/2022 12:18:42 PM
 Author:	Kitecraft
*/
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "src/CB_Config.h"
#include "src/ESPNow/CandyCraneCommands.h"
#include "src/ESPNow/EspNowManager.h"
#include "src/ESPNow/EspNowIncomingMessageQueue.h"
#include "ServoEasing.hpp"
#include <Wire.h>
#include <VL53L0X.h>

EspNowIncomingMessageQueue g_espNowMessageQueue;

ServoEasing BucketServo;
VL53L0X bucketRanger;


void setup() {
    //Serial.begin(115200);
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);
    Wire.begin(2, 0);

    Serial.println("Starting ESPNow");
    if (!InitEspNow())
    {
        Serial.println("\n\nFailed to start ESPNow stuff.  As such, I will now refuse to continue.");
        while (true) {}
    }


    bucketRanger.setTimeout(500);
    if (!bucketRanger.init())
    {
        Serial.println("Failed to detect and initialize sensor!  As such, I will now refuse to continue.");
        while (true) {}
    }

    if (BucketServo.attach(SERVO1_PIN, BUCKET_CLOSED_ANGLE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }


    OpenBucket();
    Serial.println("\n\n---\nStarting");
}

void loop() {
    HandleEspNowData();
}

void HandleEspNowData()
{
    if (!g_espNowMessageQueue.IsQueueEmpty()) {
        EspNowMessage currMessage = g_espNowMessageQueue.GetNextItem();
        switch (currMessage.command) {
        case CC_PING:
            SendEspNowCommand(CC_PONG);
            break;
        case CC_OPEN_BUCKET:
            OpenBucket();
            break;
        case CC_CLOSE_BUCKET:
            CloseBucket();
            break;
        case CC_GET_DISTANCE:
            SendSingleDistance();
            break;
        case CC_START_DISTANCE_STREAM:
            SendEspNowAck();
            break;
        case CC_END_DISTANCE_STREAM:
            SendEspNowAck();
            break;
        default:
            break;
        };
    }
}

void SendEspNowAck()
{
    SendEspNowCommand(CC_ACK_COMMAND);
}

void OpenBucket()
{
    BucketServo.easeTo(BUCKET_OPEN_ANGLE, BUCKET_OPEN_SPEED);
    while (BucketServo.isMoving())
    {
        delay(2);
    }
    //delay(BUCKET_OPEN_WAIT_TIME);
    SendEspNowCommand(CC_BUCKET_MOVE_COMPLETE);
}

void CloseBucket()
{
    BucketServo.easeTo(BUCKET_CLOSED_ANGLE, BUCKET_CLOSE_SPEED);
    while (BucketServo.isMoving())
    {
        delay(2);
    }
    SendEspNowCommand(CC_BUCKET_MOVE_COMPLETE);
}

void SendSingleDistance()
{
    uint32_t distance = bucketRanger.readRangeSingleMillimeters();
    Serial.printf("Distance: '%i'\n", distance);
    SendEspNowCommand(CC_BUCKET_DISTANCE, distance);
}