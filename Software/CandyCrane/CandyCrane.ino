/*
 Name:		CandyCrane.ino
 Created:	3/26/2022 12:17:55 PM
 Author:	Kitecraft
*/
#include <EEPROM.h>
#include <Arduino.h>
#include <WiFi.h>
#include "src/CC_Config.h"
#include "src/Crane/CraneController.h"
#include "src/ESPNow/EspNowMessageQueue.h"
#include "src/ESPNow/EspNowManager.h"
#include "src/Utilities/ButtonManager.h"
#include "src/Utilities/ButtonMap.h"

EspNowMessageQueue g_espNowMessageQueue;
CraneController g_craneController;
TaskHandle_t g_CraneControllerHandle = nullptr;
ButtonManager g_buttonManager;

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
    Serial.begin(57600);
    Wire.begin(21, 22);
    
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    g_buttonManager.Init(MCP_BUTTON_CONTROLLER_ADDRESS, MCP_BUTTON_CONTROLLER_INTERUPT_PIN);

    Serial.println("Beginning EspNow connection");
    if (!InitEspNow())
    {
        Serial.println("Failed to setup EspNow.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }

    Serial.println("Starting the crane");
    xTaskCreatePinnedToCore(CraneControllerQueueThread, 
        "Crane Queue Loop", 
        STACK_SIZE, nullptr, 
        CRANE_CONTROL_QUEUE_HANDLER_PRIORITY, 
        &g_CraneControllerQueueHandle, 
        CRANE_CONTROL_QUEUE_HANDLER_CORE);

    
    g_craneController.StartUp();
    xTaskCreatePinnedToCore(CraneControllerThread, 
        "Crane Control Loop", 
        STACK_SIZE, nullptr, 
        CRANE_CONTROL_PRIORITY, 
        &g_CraneControllerHandle, 
        CRANE_CONTROL_CORE);
        
    Serial.println("\n\n---\nBeginning Run mode.");
}


void loop() {
    
    HandleButtonPress();

    delay(1);
}

void StopAll()
{

}

void StopAllAndHome()
{

}


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
            break;
        case btn_Tower_StopHome:
            break;
        case btn_Tower_Backward:
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
            break;
        case btn_Bucket_Close:
            break;

        }
    }
}