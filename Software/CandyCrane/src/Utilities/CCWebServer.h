#pragma once
#include <Arduino.h>
#include "SPIFFS.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include "../CC_Config.h"
#include "IceFS.h"
#include "../Crane/CraneController.h"
#include "../Crane/AutoCraneSequence.h"

static WebServer server(80);
extern CraneController g_craneController;

void IRAM_ATTR WebSeverThread(void*)
{
    if (MDNS.begin(__DEVICE_NAME__)) {
        Serial.println("MDNS responder started");
    }

    server.on("/", []() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_ROOT));
        });

    server.on("/reboot", []() {
        server.sendHeader("Location", "/");
        server.send(303);
        ESP.restart();
        });

    server.on("/net", []() {
        String webPage = IceFS_ReadFile(WEBPAGE_NETWORK);
        webPage.replace("%{__DEVICE_NAME__}%", __DEVICE_NAME__);
        webPage.replace("%{ERRORTEXT}%", "");
        webPage.replace("%{NETWORKNAME}%", GetSsid());
        server.send(200, "text/html", webPage);
        });

    server.on("/setupNet", []() {
        String webPage = IceFS_ReadFile(WEBPAGE_NETWORK);
        if (server.arg("password") != server.arg("passwordConfirm"))
        {
            webPage.replace("%{ERRORTEXT}%", "Passwords did not match.<br>Changes not saved.");
        }
        else {
            SetSsid(server.arg("networkName"));
            SetSsidPassword(server.arg("password"));
            webPage.replace("%{ERRORTEXT}%", "Changes have been saved.");
        }
        webPage.replace("%{NETWORKNAME}%", GetSsid());
        server.send(200, "text/html", webPage);
        });

    server.on("/commands", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        });

    // GENERAL
    server.on("/commands/StopAll", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        g_craneController.StopAll();
        g_craneController.DisableAll();
        Serial.println("Emergency Stop");
        });

    server.on("/commands/Calibrate", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if(!g_craneController.GetAutoCraneStatus())
            g_craneController.CalibrateAll();
        });

    // TOWER
    server.on("/commands/tower/Home", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        g_craneController.MoveTowerTo(0);
        Serial.println("Homeing Tower");
        });

    server.on("/commands/tower/Stop", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.StopTowerMotion();
        Serial.println("Stopping Tower");
        });

    server.on("/commands/tower/MoveIn", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveTowerInwards();
        Serial.println("Moving Tower In");
        });

    server.on("/commands/tower/MoveOut", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveTowerOutwards();
        Serial.println("Moving Tower Out");
        });

    // DOLLY
    server.on("/commands/dolly/Home", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        g_craneController.MoveDollyTo(0);
        Serial.println("Homeing Dolly");
        });

    server.on("/commands/dolly/Stop", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.StopDolly();
        Serial.println("Stopping Dolly");
        });

    server.on("/commands/dolly/MoveIn", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveDollyInwards();
        Serial.println("Moving Dolly In");
        });

    server.on("/commands/dolly/MoveOut", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveDollyOutwards();
        Serial.println("Moving Dolly Out");
        });

    // BUCKET
    server.on("/commands/bucket/Home", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        g_craneController.MoveBucketTo(0);
        Serial.println("Homeing Bucket");
        });

    server.on("/commands/bucket/Stop", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.StopBucketMotion();
        Serial.println("Stopping Bucket");
        });

    server.on("/commands/bucket/MoveUp", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveBucketUpwards();
        Serial.println("Moving Bucket Up");
        });

    server.on("/commands/bucket/MoveDown", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.MoveBucketDownwards();
        Serial.println("Moving Bucket Down");
        });

    server.on("/commands/bucket/Open", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.OpenBucket();
        Serial.println("Opening Bucket");
        });

    server.on("/commands/bucket/Close", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        if (!g_craneController.GetAutoCraneStatus())
            g_craneController.CloseBucket();
        Serial.println("Closing Bucket");
        });


    // AUTO
    server.on("/commands/auto/Start1", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        Serial.println("Starting the sequence for Candy A");
        if (!g_craneController.GetAutoCraneStatus())
            StartAutoCrane_CandyA();
        });

    server.on("/commands/auto/Start2", [&]() {
        server.send(200, "text/html", IceFS_ReadFile(WEBPAGE_CC_CONTROL));
        Serial.println("Starting the sequence for Candy B");
        if (!g_craneController.GetAutoCraneStatus())
            StartAutoCrane_CandyB();
        });

    server.begin();
    Serial.println("HTTP server started");

    while (true)
    {
        server.handleClient();
        vTaskDelay(5);
    }
}
