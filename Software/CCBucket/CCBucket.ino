/*
 Name:		CCBucket.ino
 Created:	3/26/2022 12:18:42 PM
 Author:	Kitecraft
*/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "src/CB_Config.h"
#include "src/ESPNow/CandyCraneCommands.h"
#include <ServoEasing.hpp>
#include <Wire.h>
#include <VL53L0X.h>

//
//mac address: 84:CC:A8:83:D0:92
//

ServoEasing BucketServo;
VL53L0X bucketRanger;
bool g_bucketIsMoving = false;
WiFiClient g_tcpClient;
bool g_connectedToCrane = false;

uint32_t g_lastPing = 0;


void ConnectNetwork()
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(__DEVICE_NAME__);
    WiFi.begin(ssid, GetPassword());
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    if (!MDNS.begin(__DEVICE_NAME__)) {
        Serial.println("Error setting up MDNS responder!");
        while (1) {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");
}

void setup() {
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

    EEPROM.begin(512);
    delay(500);
    Serial.println("");
    //Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);
    //SetPassword("");
    ConnectNetwork();
    ConnectToCrane();

    Wire.begin(2, 0);
    if (!bucketRanger.init())
    {
        Serial.println("Failed to detect and initialize sensor!  As such, I will now refuse to continue.");
        //while (true) { delay(1); }
    }
    bucketRanger.setTimeout(500);
    bucketRanger.setMeasurementTimingBudget(75000);
    bucketRanger.setSignalRateLimit(0.50);
    
    if (BucketServo.attach(SERVO1_PIN, DEFAULT_BUCKET_START_ANGLE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }
    
    MoveBucket(DEFAULT_BUCKET_START_ANGLE, DEFAULT_BUCKET_SPEED);
    

    Serial.println("\n\n---\nStarting");
}

void SendMessage(CANDY_CRANE_COMMANDS command, int distance = 0, int angle = 0, int speed = 0)
{
    String message = String(command) + "," + String(distance) + "," + String(angle) + "," + String(speed);
    Serial.print("Sending: ");
    Serial.println(message);
    g_tcpClient.println(message);
}

void loop() {
    if (millis() - g_lastPing > DEFAULT_BUCKET_PING_RATE) {
        Serial.println("Lost connection to crane");
        g_tcpClient.stop();
        g_connectedToCrane = false;
        delay(2000);
        ConnectToCrane();
    }

    if (g_tcpClient.available()) {
        String message = g_tcpClient.readStringUntil('\n');
        ParseMessage(message);
    }
        
    
    if (g_bucketIsMoving)
    {
        if(!BucketServo.isMoving())
        {
            g_bucketIsMoving = false;
            SendMessage(CC_BUCKET_MOVE_COMPLETE);
        }
    }
}

void ParseMessage(String message)
{
    Serial.print("Got: '");
    Serial.print(message);
    Serial.println("'");

    int command;
    int distance;
    int angle;
    int speed;
    if (4 != sscanf(message.c_str(), "%d,%d,%d,%d", &command, &distance, &angle, &speed)) {
        return;
    }

    switch ((CANDY_CRANE_COMMANDS)command) {
    case CC_PING:
        Serial.println("Sending Pong");
        HandlePing();
        break;
    case CC_MOVE_BUCKET:
        Serial.println("Moving bucket");
        MoveBucket(angle, speed);
        break;
    case CC_GET_DISTANCE:
        Serial.println("Sending distance");
        SendSingleDistance();
        break;
    default:
        break;
    };
}

void HandlePing()
{
    g_lastPing = millis();
    SendMessage(CC_PONG);
}

void ConnectToCrane()
{
    Serial.println("Connecting to crane");
    int tryCount = 1;
    while (!g_connectedToCrane) {
        Serial.printf("Try #%i ", tryCount);
        if (!g_tcpClient.connect("CandyCrane", 8080)) {
            Serial.println("Did not connect to Candy Crane");
            tryCount++;
            delay(1000);
        }
        else {
            g_tcpClient.println("Hello to the CandyCrane");
            Serial.println("Connected to crane");
            g_connectedToCrane = true;
            g_lastPing = millis();
        }
    }
}

void MoveBucket(int angle, int speed)
{
    Serial.printf("Movenig the bucket to %i at speed %i\n", angle, speed);
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
    SendMessage(CC_BUCKET_DISTANCE, ((float)totalDistance / NUMBER_DISTANCE_SAMPLES) * 100.0);
}


