/*
 Name:		CandyComms.ino
 Created:	5/4/2022 9:31:57 AM
 Author:	Kitecraft
*/

#include "src/ESPNow/EspNowManager.h"
#include "src/ESPNow/EspNowIncomingMessageQueue.h"

EspNowIncomingMessageQueue g_espNowMessageQueue;

void setup() {
	Serial.begin(115200);
	Serial.println("");

    Serial.println("Starting ESPNow");
    if (!InitEspNow())
    {
        Serial.println("\n\nFailed to start ESPNow stuff.  As such, I will now refuse to continue.");
        while (true) { delay(1); }
    }
    Serial.println("Started");
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
            EspNowMessage newMessage = EspNowMessageFromCsvString(line);
            SendEspNowCommand(newMessage);
        }
    }
}

void HandleEspNowData()
{
    if (!g_espNowMessageQueue.IsQueueEmpty()) {
        EspNowMessage currMessage = g_espNowMessageQueue.GetNextItem();
        Serial.flush();
        Serial.println(EspNowMessageToCsvString(currMessage));
    }
}
