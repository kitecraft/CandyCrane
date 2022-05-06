#pragma once
#include <Arduino.h>
#include "../CC_Config.h"
#include "PreferencesManager.h"
#include <Preferences.h>

static String GetSsid()
{
    String ret;
    Preferences preferences;
    preferences.begin(WIFI_PREFERENCES_FILENAME, false);
    ret = preferences.getString(WIFI_PREFERENCES_SSID_KEY, "");
    preferences.end();

    return ret;
}

static void SetSsid(String s)
{
    Preferences preferences;
    preferences.begin(WIFI_PREFERENCES_FILENAME, false);
    preferences.putString(WIFI_PREFERENCES_SSID_KEY, s);
    preferences.end();
}

static String GetSsidPassword()
{
    String ret;
    Preferences preferences;
    preferences.begin(WIFI_PREFERENCES_FILENAME, false);
    ret = preferences.getString(WIFI_PREFERENCES_PASWORD_KEY, "");
    preferences.end();

    return ret;
}

static void SetSsidPassword(String s)
{
    Preferences preferences;
    preferences.begin(WIFI_PREFERENCES_FILENAME, false);
    preferences.putString(WIFI_PREFERENCES_PASWORD_KEY, s);
    preferences.end();
}
