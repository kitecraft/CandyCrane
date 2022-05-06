#pragma once
#include <Arduino.h>
#include "SPIFFS.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include "../CC_Config.h"

void IRAM_ATTR WebSeverThread(void*)
{
    static WebServer server(80);
    if (MDNS.begin(__DEVICE_NAME__)) {
        Serial.println("MDNS responder started");
    }

    server.on(F("/"), []() {
        server.send(200, "text/plain", "hello from esp32!");
        });

    server.on(UriBraces("/users/{}"), []() {
        String user = server.pathArg(0);
        server.send(200, "text/plain", "User: '" + user + "'");
        });

    server.begin();
    Serial.println("HTTP server started");

    while (true)
    {
        server.handleClient();
        vTaskDelay(5);
    }
}