#pragma once
#include <Arduino.h>
#include <functional>
#include <stdlib.h>
#include "MultiTinyStepper_Config.h"

typedef std::function<void(uint8_t, uint8_t, bool)> processStepHandler;
class MtsStepper
{
private:
    bool _reversed = false;
    void DeterminePeriodOfNextStep();
    processStepHandler processStep = nullptr;
    MTS_STEPPER_STEP_SIZE _stepSize;
    MTS_STEPPER_TYPE _stepperType;
    void setNextStep();

    int8_t _stepPhase;
    float _stepsPerMillimeter;
    float _stepsPerRevolution;
    int _directionOfMotion;
    long _currentPosition_InSteps;
    long _targetPosition_InSteps;
    float _desiredSpeed_InStepsPerSecond;
    float _desiredPeriod_InUSPerStep;
    float _acceleration_InStepsPerSecondPerSecond;
    float _acceleration_InStepsPerUSPerUS;
    float _periodOfSlowestStep_InUS;
    float _minimumPeriodForAStoppedMotion;
    float _nextStepPeriod_InUS;
    unsigned long _lastStepTime_InUS;
    float _currentStepPeriod_InUS;
    
    

public:
    MtsStepper();

    /*
    Sets the stepsPerRevolution for the appropriate stepper type
    MTS_STEPPER_TYPE_64 = 2038
    MTS_STEPPER_TYPE_16 = 513

    */
    void setStepperType(MTS_STEPPER_TYPE type, MTS_STEPPER_STEP_SIZE stepSize);
    void setReversed(bool reversed) { _reversed = reversed; }

    // Functions which use Steps

    void setSpeedInStepsPerSecond(float speedInStepsPerSecond);
    void setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond);
    void setCurrentPositionInSteps(long currentPositionInSteps);
    long getCurrentPositionInSteps();
    void setTargetPositionRelativeInSteps(long distanceToMoveInSteps);
    void setTargetPositionInSteps(long absolutePositionToMoveToInSteps);
    float getCurrentVelocityInStepsPerSecond();


    // Functions which use millimeters

    void setStepsPerMillimeter(float motorStepPerMillimeter);
    float getCurrentPositionInMillimeters();
    void setCurrentPositionInMillimeters(float currentPositionInMillimeters);
    void setSpeedInMillimetersPerSecond(float speedInMillimetersPerSecond);
    void setAccelerationInMillimetersPerSecondPerSecond(float accelerationInMillimetersPerSecondPerSecond);
    bool moveToHomeInMillimeters(long directionTowardHome, float speedInMillimetersPerSecond, long maxDistanceToMoveInMillimeters, int homeLimitSwitchPin);
    void setTargetPositionRelativeInMillimeters(float distanceToMoveInMillimeters);
    void setTargetPositionInMillimeters(float absolutePositionToMoveToInMillimeters);
    float getCurrentVelocityInMillimetersPerSecond();


    // Functions which use revolutions

    void setStepsPerRevolution(float motorStepPerRevolution);
    void setCurrentPositionInRevolutions(float currentPositionInRevolutions);
    float getCurrentPositionInRevolutions();
    void setSpeedInRevolutionsPerSecond(float speedInRevolutionsPerSecond);
    void setAccelerationInRevolutionsPerSecondPerSecond(float accelerationInRevolutionsPerSecondPerSecond);
    bool moveToHomeInRevolutions(long directionTowardHome, float speedInRevolutionsPerSecond, long maxDistanceToMoveInRevolutions, int homeLimitSwitchPin);
    void setTargetPositionRelativeInRevolutions(float distanceToMoveInRevolutions);
    void setTargetPositionInRevolutions(float absolutePositionToMoveToInRevolutions);
    float getCurrentVelocityInRevolutionsPerSecond();



    // General Functions

    //Returns true when move is complete
    bool motionComplete();

    /*
    Acts as a way to calibrate a home position based on a switch.
    Max distance acts as a safty.
    This will block until complete.
    */
    bool moveToHomeInSteps(long directionTowardHome, float speedInStepsPerSecond, long maxDistanceToMoveInSteps, int homeSwitchPin);

    //Stops the stepper immediatly.
    void deadStop();

    //Turn off the current to the motor
    void disable();

    //Call this when needing to move.  Returns true if movement complete. 
    bool process();


    /*
    DO NOT CALL THIS! It is used by MultiTinyStepper
    */
    void setCallback(processStepHandler fn) { processStep = fn; }
};

