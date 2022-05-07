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

TaskHandle_t g_CCCommsHandle = nullptr;
void IRAM_ATTR CCComsThread(void*)
{
    while (true) {
        if (Serial2.available()) {
            String line = Serial2.readStringUntil('\n');
            //Serial.print("Got line: '");
            //Serial.print(line);
            //Serial.println("'");
            g_incomeingMessageQueue.AddItemToQueue(EspNowMessageFromCsvString(line));
        }
        vTaskDelay(1);
    }
}

bool setupComplete = true;
void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    StartNetworkStuff();

    Serial.println("Starting the comms");
    xTaskCreatePinnedToCore(CCComsThread,
        "CCComms Loop",
        STACK_SIZE, nullptr,
        CANDY_CRANE_COMMS_HANDLER_PRIORITY,
        &g_CCCommsHandle,
        CANDY_CRANE_COMMS_CORE);

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