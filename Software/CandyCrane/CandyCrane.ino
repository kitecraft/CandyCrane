/*
 Name:		CandyCrane.ino
 Created:	3/26/2022 12:17:55 PM
 Author:	Kitecraft
*/
//#include <ServoEasing.h>
#include <Arduino.h>
#include <Wire.h>
#include <ServoEasing.hpp>
#include "src/Utilities/Network.h"
#include "src/Utilities/CCWebServer.h"
#include "src/CC_Config.h"
#include "src/Crane/CraneController.h"
#include "src/Utilities/OTAHandler.h"
#include "src/Crane/AutoCraneSequence.h"


CraneController g_craneController;
void (*g_AutoCraneStepList[])() = { AutoCrane_Step1, autoCrane_Step2, autoCrane_Step3, autoCrane_Step4, autoCrane_Step5, autoCrane_Step6, autoCrane_Step7 };

TaskHandle_t g_CraneControllerHandle = nullptr;
TaskHandle_t g_webServerHandle = nullptr;
TaskHandle_t g_OTAHandle = nullptr;

void IRAM_ATTR CraneControllerThread(void*)
{
    g_craneController.Run();
}

bool setupComplete = true;

void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    Wire.begin();
    Wire.setClock(700000);

    StartNetworkStuff();
    Serial.println(WiFi.macAddress());

    Serial.println("Starting the crane");
    if (!g_craneController.StartUp())
    {
        Serial.println("Failed to start crane controller.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }
    xTaskCreate(CraneControllerThread, 
        "Crane Control Loop", 
        STACK_SIZE, nullptr, 
        CRANE_CONTROL_PRIORITY, 
        &g_CraneControllerHandle);
}


#define DIR_OUT true
#define DIR_IN false
bool currentAutoMode = true;
bool dollyDirection = DIR_OUT;
bool towerDirection = DIR_OUT;

void loop() {
    
    if (g_craneController.GetAutoCraneStatus()) {
        //Serial.printf("Crane is in auto mode: %i\n", g_AutoCraneCurrentStep);
        g_AutoCraneStepList[g_AutoCraneCurrentStep]();
    }
    




    /*
    else {
        if (currentAutoMode) {
            StartAutoCrane_CandyA();
        }
        else {
            StartAutoCrane_CandyB();
        }
        currentAutoMode = !currentAutoMode;
    }
    */


    /*
    if (!g_craneController.IsDollyInMotion())
    {
        if (dollyDirection == DIR_OUT) {
            g_craneController.MoveDollyOutwards();
        }
        else {
            g_craneController.MoveDollyInwards();
        }
        dollyDirection = !dollyDirection;
    }

    if (!g_craneController.IsTowerInMotion())
    {
        if (towerDirection == DIR_OUT) {
            g_craneController.MoveTowerOutwards();
        }
        else {
            g_craneController.MoveTowerInwards();
        }
        towerDirection = !towerDirection;
    }
    */
    
}



void StartNetworkStuff()
{
    if (!connectToNetwork())
    {
        startLocalNetwork();
        Serial.println("NOT starting OTA handler");
    }
    else {
        Serial.println("Starting OTA Handler");
        xTaskCreate(OTAThread, "OTA Loop", STACK_SIZE, nullptr, OTA_PRIORITY, &g_OTAHandle);
    }

    if (MDNS.begin(__DEVICE_NAME__)) {
        Serial.println("MDNS responder started");
    }
    Serial.println("Starting Webserver");
    xTaskCreate(WebSeverThread, "WebServer Loop", STACK_SIZE, nullptr, WEB_SERVER_PRIORITY, &g_webServerHandle);
}
