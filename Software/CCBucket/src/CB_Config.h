#pragma once
#include <EEPROM.h>

#define __DEVICE_NAME__ "CCBucket"
#define __DEVICE_VERSION__ "0.1"

String ssid = "Starside";

#define SERVO1_PIN 3

#define DEFAULT_BUCKET_START_ANGLE 40
#define DEFAULT_BUCKET_SPEED 60
#define NUMBER_DISTANCE_SAMPLES 10.0


int addr = 0;
static void SetPassword(String word) {
	for (int i = 0; i < word.length(); ++i) {
		EEPROM.write(i, word[i]);
	}
	EEPROM.write(word.length(), '\0');
	if (!EEPROM.commit()) {
		Serial.println("ERROR! EEPROM commit failed");
	}
}

static String GetPassword() {
	String word = "";
	char readChar = 'a';
	int i = 0;
	while (readChar != '\0') {
		readChar = char(EEPROM.read(i));
		delay(10);
		i++;
		if (readChar != '\0') {
			word += readChar;
		}
	}

	return word;
	
}