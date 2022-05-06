#pragma once
#include <Arduino.h>
#include "SPIFFS.h"
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>
#include "../CC_Config.h"
#include "IceFS.h"
#include "../Crane/CraneController.h"

static WebServer server(80);
extern CraneController g_craneController;

void IRAM_ATTR WebSeverThread(void*)
{
    if (MDNS.begin(__DEVICE_NAME__)) {
        Serial.println("MDNS responder started");
    }

    server.on(F("/"), []() {
        server.send(200, "text/plain", "hello from esp32!");
        });

    server.on(F("/reboot"), []() {
        ESP.restart();
        });

    server.on(F("/net"), []() {
        String webPage = IceFS_ReadFile(WEBPAGE_NETWORK);
        webPage.replace("%{__DEVICE_NAME__}%", __DEVICE_NAME__);
        webPage.replace("%{ERRORTEXT}%", "");
        webPage.replace("%{NETWORKNAME}%", GetSsid());
        server.send(200, "text/html", webPage);
        });

    server.on(F("/setupNet"), []() {
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


    server.on(F("/commands/tower/Home"), [&]() {
        server.send(200, "text/plain", "Homeing");
        });

    server.on(F("/commands/tower/Stop"), [&]() {
        g_craneController.StopTowerMotion();
        server.send(200, "text/plain", "Stopped Tower");
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