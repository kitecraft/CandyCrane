#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "../CCC_Config.h"
#include "EspNowMessage.h"
#include "EspNowIncomingMessageQueue.h"

constexpr char WIFI_SSID[] = "Starside";
static uint8_t broadcastAddress[] = { 0x84, 0xCC, 0xA8, 0x83, 0xD0, 0x92 };  //Bucket
extern EspNowIncomingMessageQueue g_espNowMessageQueue;

void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
    /*
    if (sendStatus == 0) {
        Serial.println("Delivery success");
    }
    else {
        Serial.printf("Last Packet Send Status: Delivery fail  '%i'\n", sendStatus);
    }
    */
}

void OnDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
    //Serial.println("got data");
    EspNowMessage message;
    memcpy(&message, incomingData, sizeof(message));
    g_espNowMessageQueue.AddItemToQueue(message);
    //Serial.println(EspNowMessageToCsvString(message));
    
}

int32_t getWiFiChannel(const char* ssid) {
    if (int32_t n = WiFi.scanNetworks()) {
        for (uint8_t i = 0; i < n; i++) {
            if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
                return WiFi.channel(i);
            }
        }
    }
    return 0;
}

static bool InitEspNow()
{
    WiFi.mode(WIFI_STA);
    int32_t channel = getWiFiChannel(WIFI_SSID);
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return false;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_CONTROLLER, channel, NULL, 0);

    return true;
}

static void SendEspNowCommand(EspNowMessage espNowOutgoingMessage)
{
    esp_now_send(broadcastAddress, (uint8_t*)&espNowOutgoingMessage, sizeof(espNowOutgoingMessage));
}