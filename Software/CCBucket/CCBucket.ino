/*
 Name:		CCBucket.ino
 Created:	3/26/2022 12:18:42 PM
 Author:	Kitecraft
*/
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "ServoEasing.hpp"
#include <Wire.h>
#include <VL53L0X.h>

enum CANDY_CRANE_COMMANDS {
    CC_NONE = -1,
    CC_OPEN_BUCKET,
    CC_CLOSE_BUCKET,
    CC_BUCKET_MOVE_COMPLETE,
    CC_GET_DISTANCE,
    CC_START_DISTANCE_STREAM,
    CC_END_DISTANCE_STREAM,

    CC_ACK_COMMAND,
};


/*  Start ESPNOW() stuff */
uint8_t broadcastAddress[] = { 0x3C, 0x71, 0xBF, 0x44, 0x7B, 0x68};

typedef struct struct_message {
    int command = CC_NONE;
    uint16_t value = -1;
} struct_message;
struct_message espNowIncomingMessage;
bool espNowDataReceived = false;

void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
    //Serial.print("Last Packet Send Status: ");
    if (sendStatus == 0) {
        //Serial.println("Delivery success");
    }
    else {
        Serial.println("Last Packet Send Status: Delivery fail");
    }
}

void OnDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
    memcpy(&espNowIncomingMessage, incomingData, sizeof(espNowIncomingMessage));

    espNowDataReceived = true;
}
/*  end ESPNOW() stuff */


#define BUCKET_CLOSED_ANGLE  127
#define BUCKET_OPEN_ANGLE 39
#define BUCKET_OPEN_SPEED 240
#define BUCKET_OPEN_WAIT_TIME 500
#define BUCKET_CLOSE_SPEED 80
#define BUCKET_CLOSE_WAIT_TIME 1500
#define SERVO1_PIN 3

ServoEasing BucketServo;
VL53L0X bucketRanger;


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Wire.begin(2, 0);

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);


    bucketRanger.setTimeout(500);
    if (!bucketRanger.init())
    {
        Serial.println("Failed to detect and initialize sensor!");
        while (1) {}
    }

    if (BucketServo.attach(SERVO1_PIN, BUCKET_CLOSED_ANGLE) == INVALID_SERVO) {
        Serial.println(F("Error attaching servo"));
    }


    CloseBucket();
    Serial.println("\n\n---\nStarting");
}

void loop() {
    if (espNowDataReceived)
    {
        HandleEspNowData();
        espNowDataReceived = false;
    }
}

void HandleEspNowData()
{
    CANDY_CRANE_COMMANDS currCommand = static_cast<CANDY_CRANE_COMMANDS>(espNowIncomingMessage.command);
    switch (currCommand) {
    case CC_OPEN_BUCKET:
        Serial.println("Opening");
        OpenBucket();
        SendEspNowAck();
        break;
    case CC_CLOSE_BUCKET:
        Serial.println("Closing");
        CloseBucket();
        SendEspNowAck();
        break;
    case CC_GET_DISTANCE:
        SendEspNowAck();
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

void SendEspNowAck()
{
    struct_message commandResult;
    commandResult.command = CC_ACK_COMMAND;
    esp_now_send(broadcastAddress, (uint8_t*)&commandResult, sizeof(commandResult));
}

void OpenBucket()
{
    BucketServo.easeTo(BUCKET_OPEN_ANGLE, BUCKET_OPEN_SPEED);
    delay(BUCKET_OPEN_WAIT_TIME);
}

void CloseBucket()
{
    BucketServo.easeTo(BUCKET_CLOSED_ANGLE, BUCKET_CLOSE_SPEED);
    delay(BUCKET_CLOSE_WAIT_TIME);
}

void SendSingleDistance()
{
    struct_message commandResult;
    commandResult.command = CC_GET_DISTANCE;
    commandResult.value = bucketRanger.readRangeSingleMillimeters();
    esp_now_send(broadcastAddress, (uint8_t*)&commandResult, sizeof(commandResult));
}