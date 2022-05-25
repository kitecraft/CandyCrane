#pragma once
#include <Arduino.h>
#include "../CC_Config.h"

extern CraneController g_craneController;
static int g_AutoCraneCurrentStep = -1;

static void StartAutoCrane(int candyOption)
{
    int moveTo = 0;
    if (candyOption == 1)
    {
        moveTo = 45;
    }
    else if (candyOption == 2) {
        moveTo = 145;
    }
    else {
        return;
    }

    g_AutoCraneCurrentStep = 0;
    g_craneController.MoveDollyTo(190);
    vTaskDelay(3000);
    g_craneController.MoveTowerTo(moveTo);
    g_craneController.SetAutoCraneStatus(true);
}

static void StartAutoCrane_CandyA()
{
    StartAutoCrane(1);
}

static void StartAutoCrane_CandyB()
{
    StartAutoCrane(2);
}


static void AutoCrane_Step1()
{
    if (!g_craneController.IsDollyInMotion() && !g_craneController.IsTowerInMotion())
    {
        g_craneController.MoveBucketTo(158);
        vTaskDelay(2000);
        g_craneController.OpenBucketAsync();
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step2()
{
    if (!g_craneController.IsBucketInMotion())
    {
        g_craneController.CloseBucketAsync();
        vTaskDelay(750);
        g_craneController.MoveBucketTo(20);
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step3()
{
    if (!g_craneController.IsBucketInMotion())
    {
        g_craneController.MoveTowerTo(TOWER_MAXIMUM_DISTANCE);
        g_craneController.MoveDollyTo(30);
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step4()
{
    if (!g_craneController.IsTowerInMotion() && !g_craneController.IsDollyInMotion())
    {
        g_craneController.MoveBucketTo(126);
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step5()
{
    if (!g_craneController.IsBucketInMotion())
    {
        g_craneController.OpenBucket();
        g_craneController.MoveBucketTo(30);
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step6()
{
    if (!g_craneController.IsBucketInMotion())
    {
        g_craneController.MoveTowerTo(0);
        g_craneController.MoveDollyTo(0);
        g_craneController.MoveBucketTo(0);
        g_craneController.CloseBucketAsync();
        g_AutoCraneCurrentStep++;
    }
}

static void autoCrane_Step7()
{
    if (!g_craneController.IsTowerInMotion() && !g_craneController.IsDollyInMotion())
    {
        //g_craneController.CalibrateAll();
        g_craneController.SetAutoCraneStatus(false);
        g_AutoCraneCurrentStep == -1;
    }
}