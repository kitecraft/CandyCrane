/*
 Name:		CCBucket.ino
 Created:	3/26/2022 12:18:42 PM
 Author:	Kitecraft
*/
#include <espnow.h>
#include "src/CB_Config.h"
#include "src/ESPNow/CandyCraneCommands.h"
#include "src/ESPNow/EspNowManager.h"
#include "src/ESPNow/EspNowIncomingMessageQueue.h"
#include <ServoEasing.hpp>
#include <Wire.h>
#include <VL53L0X.h>

//
//mac address: 
//

EspNowIncomingMessageQueue g_espNowMessageQueue;

ServoEasing BucketServo;
VL53L0X bucketRanger;
bool g_bucketIsMoving = false;


void setup() {
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    //Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    Wire.begin(2, 0);

    if (!bucketRanger.init())
    {
        Serial.println("Failed to detect and initialize sensor!  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }
    bucketRanger.setTimeout(500);
    bucketRanger.setMeasurementTimingBudget(75000);
    bucketRanger.setSignalRateLimit(0.50);
    
    if (BucketServo.attach(SERVO1_PIN, DEFAULT_BUCKET_START_ANGLE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }
    
    //MoveBucket(DEFAULT_BUCKET_OPEN_ANGLE, DEFAULT_BUCKET_OPEN_SPEED);
    

    //Serial.println("Starting ESPNow");
    if (!InitEspNow())
    {
        Serial.println("\n\nFailed to start ESPNow stuff.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }

    //Serial.println("\n\n---\nStarting");
}

void loop() {
    HandleEspNowData();
    
    if (g_bucketIsMoving)
    {
        if(!BucketServo.isMoving())
        {
            g_bucketIsMoving = false;
            SendEspNowCommand(CC_BUCKET_MOVE_COMPLETE);
        }
    }
}

void HandleEspNowData()
{
    if (!g_espNowMessageQueue.IsQueueEmpty()) {
        EspNowMessage currMessage = g_espNowMessageQueue.GetNextItem();
        //Serial.println("Handling ESP Now data");
        //Serial.print("Command: '");
        //Serial.print(currMessage.command);
        //Serial.println("'");
        switch (currMessage.command) {
        case CC_PING:
            //Serial.println("Sending Pong");
            SendEspNowCommand(CC_PONG);
            break;
        case CC_MOVE_BUCKET:
            //Serial.println("Moving bucket");
            MoveBucket(currMessage.angle, currMessage.speed);
            break;
        case CC_GET_DISTANCE:
            //Serial.println("Sending distance");
            SendSingleDistance();
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

void MoveBucket(int angle, int speed)
{
    BucketServo.easeTo(angle, speed);
    g_bucketIsMoving = true;
};

void SendSingleDistance()
{
    long totalDistance = 0;
    for (int i = 0; i < NUMBER_DISTANCE_SAMPLES; i++) {
        totalDistance += bucketRanger.readRangeSingleMillimeters();
        delay(20);
    }
    SendEspNowDistance(totalDistance/ NUMBER_DISTANCE_SAMPLES);
    //SendEspNowDistance(bucketRanger.readRangeSingleMillimeters());
}


