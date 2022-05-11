#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../CC_Config.h"
#include "PreferencesManager.h"

static bool connectToNetwork()
{
    WiFi.hostname(__DEVICE_NAME__);
    WiFi.mode(WIFI_STA);
    //SetSsid("");
    //SetSsidPassword("");

    if (GetSsid().equals(""))
    {
        return false;
    }

    Serial.print("Connecting Network: ");

    int tryCount = 1;
    bool connected = false;
    while (tryCount <= NETWORK_TRY_TIMES) {
        Serial.printf("Try #%i to connect to network '%s'\n", tryCount, GetSsid());
        //WiFi.disconnect(true);
        WiFi.begin(GetSsid().c_str(), GetSsidPassword().c_str(), 1);
        int waitCount = 0;
        while (waitCount < NETWORK_WAIT_TIME_PER_TRY * 2) {
            if (WiFi.status() == WL_CONNECTED) {
                Serial.print("Connected to network: " + GetSsid() + " - ");
                Serial.println(WiFi.localIP());
                return true;
            }
            delay(500);
            waitCount++;
        }
        tryCount++;
    }
    return false;
}

static void startLocalNetwork()
{
    Serial.print("Failed to connect to network.  Createing local: ");
    IPAddress IP = WiFi.softAP(__DEVICE_NAME__);
    Serial.println(WiFi.softAPIP());
}