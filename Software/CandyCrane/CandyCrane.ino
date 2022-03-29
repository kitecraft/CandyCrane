/*
 Name:		CandyCrane.ino
 Created:	3/26/2022 12:17:55 PM
 Author:	Kitecraft
*/
#include <Arduino.h>
#include <WiFi.h>
#include "src/CC_Config.h"
//#include "src/Network/Network.h"
//#include "src/Network/CCWebServer.h"
//#include "src/Network/OTAManager.h"
#include "src/ESPNow/EspNowManager.h"
#include "src/ESPNow/EspNowIncomingMessageQueue.h"
#include "src/Crane/CraneController.h"

TaskHandle_t g_webServerHandler = nullptr;
TaskHandle_t g_OTAHandler = nullptr;

EspNowIncomingMessageQueue g_espNowMessageQueue;

CraneController g_craneController;
TaskHandle_t g_CraneControllerHandle = nullptr;
void IRAM_ATTR CraneControllerThread(void*)
{
    g_craneController.Run();
}
TaskHandle_t g_CraneControllerQueueHandle = nullptr;
void IRAM_ATTR CraneControllerQueueThread(void*)
{
    g_craneController.RunQueueHandler();
}


/*
#define NEXT_BUCKET_ACTION_DELAY 3000
unsigned long nextBucketAction = 0;
bool ccBucketStatus = false;

#define NEXT_GET_DISTANCE_ACTION_DELAY 5000
unsigned long nextGetDistanceAction = 0;
*/

bool setupComplete = true;
void setup() {
    Serial.begin(115200);
    Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);

    /*
    Serial.println("Beginning network connection");
    if (!connectToNetwork())
    {
        Serial.println("Failed to connect to network.  Start local webserver");
        setupComplete = false;

        xTaskCreatePinnedToCore(handleLocalWebSever, 
            "WebServer Loop", 
            STACK_SIZE, 
            nullptr, 
            WEB_SERVER_PRIORITY, 
            &g_webServerHandler, 
            WEBSERVER_CORE);
        //display error here
    }
    else {
        Serial.println("Connected to network succesfully.  Starting webserver.");

        xTaskCreatePinnedToCore(handleWebSever, 
            "WebServer Loop", 
            STACK_SIZE, 
            nullptr, 
            WEB_SERVER_PRIORITY, 
            &g_webServerHandler, 
            WEBSERVER_CORE);

        Serial.println("Started webserver handler.  Starting OTA Handler");

        xTaskCreatePinnedToCore(handleOTA, 
            "OTA Loop", 
            STACK_SIZE, 
            nullptr, 
            OTA_PRIORITY, 
            &g_OTAHandler, 
            OTA_CORE);

        Serial.println("Started OTA handler");
    }
    delay(2000);
    */


    Serial.println("Beginning EspNow connection");
    if (!InitEspNow())
    {
        Serial.println("Failed to setup EspNow.  As such, I will now refuse to continue.");
        while (true) {}
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

    Serial.println("\n\n---\nBeginning Run mode.  Don't get fat!");
}


void loop() {
    //HandleEspNowData();

    /*
    if (nextBucketAction < millis())
    {
        if (ccBucketStatus)
        {
            Serial.print("Sending Close... ");
            SendEspNowCommand(CC_CLOSE_BUCKET);
        }
        else
        {
            Serial.print("Sending Open... ");
            SendEspNowCommand(CC_OPEN_BUCKET);
        }
        
        ccBucketStatus = !ccBucketStatus;
        nextBucketAction = millis() + NEXT_BUCKET_ACTION_DELAY;
        Serial.println("Command has been sent");
    }

    if (nextGetDistanceAction < millis())
    {
        Serial.print("Requesting distance... ");
        SendEspNowCommand(CC_GET_DISTANCE);
        Serial.println("Distance Requested");
        nextGetDistanceAction = millis() + NEXT_GET_DISTANCE_ACTION_DELAY;
    }
    */
}

void HandleEspNowData()
{
    if (!g_espNowMessageQueue.IsQueueEmpty())
    {
        EspNowMessage message = g_espNowMessageQueue.GetNextItem();
        switch (message.command) {
        case CC_ACK_COMMAND:
            Serial.println("Acked");
            break;
        case CC_GET_DISTANCE:
            Serial.printf("Distance: '%i'", message.value);
            break;
        default:
            break;
        };
    }
}
