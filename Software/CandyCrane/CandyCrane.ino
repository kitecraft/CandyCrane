/*
 Name:		CandyCrane.ino
 Created:	3/26/2022 12:17:55 PM
 Author:	Kitecraft
*/
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "src/Utilities/Network.h"
#include "src/Utilities/CCWebServer.h"
#include "src/CC_Config.h"
#include "src/Crane/CraneController.h"
#include "src/Utilities/IncomeingMessageQueue.h"
#include "src/Utilities/OTAHandler.h"


bool g_otaRunning = false;

IncomeingMessageQueue g_incomeingMessageQueue;
CraneController g_craneController;

TaskHandle_t g_CraneControllerHandle = nullptr;
TaskHandle_t g_webServerHandle = nullptr;
TaskHandle_t g_OTAHandle = nullptr;

void IRAM_ATTR CraneControllerThread(void*)
{
    g_craneController.Run();
}
TaskHandle_t g_CraneControllerQueueHandle = nullptr;
void IRAM_ATTR CraneControllerQueueThread(void*)
{
    g_craneController.RunQueueHandler();
}

bool setupComplete = true;
void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    StartNetworkStuff();

    Serial.println("Starting the crane");
    xTaskCreatePinnedToCore(CraneControllerQueueThread,
        "Crane Queue Loop",
        STACK_SIZE, nullptr,
        CRANE_CONTROL_QUEUE_HANDLER_PRIORITY,
        &g_CraneControllerQueueHandle,
        CRANE_CONTROL_QUEUE_HANDLER_CORE);

    if (!g_craneController.StartUp())
    {
        Serial.println("Failed to start crane controller.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }

    xTaskCreatePinnedToCore(CraneControllerThread, 
        "Crane Control Loop", 
        STACK_SIZE, nullptr, 
        CRANE_CONTROL_PRIORITY, 
        &g_CraneControllerHandle, 
        CRANE_CONTROL_CORE);
      
    Serial.println("\n\n---\nBeginning Run mode.");
}


void loop() {
    HandleInput();
}

void HandleInput()
{
    if (Serial2.available()) {
        String line = Serial2.readStringUntil('\n');
        Serial.print("Got line: '");
        Serial.print(line);
        Serial.println("'");
        g_incomeingMessageQueue.AddItemToQueue(EspNowMessageFromCsvString(line));
    }
}

void StopAll()
{

}

void StopAllAndHome()
{

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
        xTaskCreatePinnedToCore(OTAThread, "OTA Loop", STACK_SIZE, nullptr, OTA_PRIORITY, &g_OTAHandle, OTA_CORE);
    }

    Serial.println("Starting Webserver");
    xTaskCreatePinnedToCore(WebSeverThread, "WebServer Loop", STACK_SIZE, nullptr, WEB_SERVER_PRIORITY, &g_webServerHandle, WEBSERVER_CORE);
}

/*
void HandleButtonPress()
{
    int button = g_buttonManager.HandleButtonInterrupt();

    if (button != NO_PIN)
    {
        Serial.printf("Button pressed: '%i'\n", button);
        switch (button) {
        case btn_Emergency_StopHome:
            break;
        case btn_Tower_Forward:
            g_craneController.MoveTowerOutwards();
            break;
        case btn_Tower_StopHome:
            g_craneController.StopTowerMotion();
            break;
        case btn_Tower_Backward:
            g_craneController.MoveTowerInwards();
            break;
        case btn_Dolly_Outwards:
            g_craneController.MoveDollyOutwards();
            break;
        case btn_Dolly_StopHome:
            g_craneController.IsDollyInMotion() ?
                g_craneController.StopDolly() : 
                g_craneController.RecalibrateDolly();
            break;
        case btn_Dolly_Inwards:
            g_craneController.MoveDollyInwards();
            break;
        case btn_Bucket_Up:
            g_craneController.MoveBucketUpwards();
            break;
        case btn_Bucket_StopHome:
            g_craneController.StopBucketMotion();
            break;
        case btn_Bucket_Down:
            g_craneController.MoveBucketDownwards();
            break;
        case btn_Bucket_Open:
            Serial.println("Opening Bucket");
            g_craneController.OpenBucket();
            break;
        case btn_Bucket_Close:
            Serial.println("Closing Closing");
            g_craneController.CloseBucket();
            break;

        }
    }
}
*/