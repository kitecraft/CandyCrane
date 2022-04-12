/*
 Name:		CCComms.ino
 Created:	4/8/2022 1:13:00 PM
 Author:	Kitecraft
*/

#include "src/CCComms_Config.h"
#include "src/ESPNow/EspNowManager.h"
#include "src/ESPNow/EspNowIncomingMessageQueue.h"
//#include <SoftwareSerial.h>

//SoftwareSerial swSer;
EspNowIncomingMessageQueue g_espNowMessageQueue;

void setup() {
    Serial.begin(57600);
    //Serial.printf("\n\n----- %s v%s -----\n\n", __DEVICE_NAME__, __DEVICE_VERSION__);
    //swSer.begin(57600, SWSERIAL_8N1, 2, 0);
    
    if (!InitEspNow())
    {
        Serial.println("\n\nFailed to start ESPNow stuff.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }
    //swSer.flush();
}


void loop() {
    HandleSerialCommsData();
    HandleEspNowData();
}

void HandleSerialCommsData()
{
    if (Serial.available()) {
        while (Serial.available()) {
            String line = Serial.readStringUntil('\n');
            EspNowMessage newMessage(line);
            SendEspNowCommand(newMessage);
        }
    }
}

void HandleEspNowData()
{
    if (!g_espNowMessageQueue.IsQueueEmpty()) {
        
        EspNowMessage currMessage = g_espNowMessageQueue.GetNextItem();
        Serial.flush();
        Serial.println(currMessage.ToString());
    }
}