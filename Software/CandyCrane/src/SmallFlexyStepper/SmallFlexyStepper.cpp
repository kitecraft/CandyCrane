
//      ******************************************************************
//      *                                                                *
//      *                    SmallFlexyStepper Motor Driver              *
//      *                                                                *
//      *            Kitecraft                     05/10/2022            *
//      *               Copyright (c) Kitecraft, 2022                    *
//      *                                                                *
//      ******************************************************************


// MIT License
// 
// Copyright (c) 2022 Kitecraft.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


//
// This library is used to control one or more stepper motors.  It requires a 
// ULN2003 stepper driver board.  The motors are 
// accelerated and decelerated as they travel to the final position.  This driver
// supports changing the target position, speed or rate of acceleration while a
// motion is in progress.
//
// Because the library allows making changes while moving, it can not generate
// as fast of a step rate as a driver that requires each motion to complete.
//
// This stepper motor driver is based on the original FlexyStepper project
// by S. Reifel & Co 
//
//
//
// Usage:
//    Near the top of the program, add:
//        include "SmallFlexyStepper.h"
//
//    For each stepper, declare a global object outside of all functions as 
//    follows:
//        SmallFlexyStepper stepper1;
//        SmallFlexyStepper stepper2;
//
//    In Setup(), assign pin numbers:
//        stepper1.connectToPins(8,9,10,11);
//
//    Notes: 
//        * Most stepper motors have either 513 or 4096 steps per revolution.//
//
//    Move one motor in units of steps:
//        //
//        // set the speed in steps/second and acceleration in steps/second/second
//        //
//        stepper1.setSpeedInStepsPerSecond(100);
//        stepper1.setAccelerationInStepsPerSecondPerSecond(100);
//
//        //
//        // move 200 steps in the backward direction
//        //
//        stepper1.moveRelativeInSteps(-200);
//
//        //
//        // move to an absolute position of 200 steps
//        //
//        stepper1.moveToPositionInSteps(200);
//
//
//    Move one motor in units of revolutions:
//        //
//        // set the number of steps per revolutions, 200 
//        //
//        stepper1.setStepsPerRevolution(200);
//
//        //
//        // set the speed in rotations/second and acceleration in 
//        // rotations/second/second
//        //
//        stepper1.setSpeedInRevolutionsPerSecond(1);
//        stepper1.setAccelerationInRevolutionsPerSecondPerSecond(1);
//
//        //
//        // move backward 1.5 revolutions
//        //
//        stepper1.moveRelativeInRevolutions(-1.5);
//
//        //
//        // move to an absolute position of 3.75 revolutions
//        //
//        stepper1.moveToPositionInRevolutions(3.75);
//
//
//    Move one motor in units of millimeters:
//        //
//        // set the number of steps per millimeter
//        //
//        stepper1.setStepsPerMillimeter(25);
//
//        //
//        // set the speed in millimeters/second and acceleration in 
//        // millimeters/second/second
//        //
//        stepper1.setSpeedInMillimetersPerSecond(20);
//        stepper1.setAccelerationInMillimetersPerSecondPerSecond(20);
//
//        //
//        // move backward 15.5 millimeters
//        //
//        stepper1.moveRelativeInMillimeters(-15.5);
//
//        //
//        // move to an absolute position of 125 millimeters
//        //
//        stepper1.moveToPositionInMillimeters(125);
//
//
//    Move two motors in units of revolutions:
//        //
//        // set the number of steps per revolutions, 200 with no microstepping, 
//        // 800 with 4x microstepping
//        //
//        stepper1.setStepsPerRevolution(200);
//        stepper2.setStepsPerRevolution(200);
//
//        //
//        // set the speed in rotations/second and acceleration in 
//        // rotations/second/second
//        //
//        stepper1.setSpeedInRevolutionsPerSecond(1);
//        stepper1.setAccelerationInRevolutionsPerSecondPerSecond(1);
//        stepper2.setSpeedInRevolutionsPerSecond(1);
//        stepper2.setAccelerationInRevolutionsPerSecondPerSecond(1);
//
//        //
//        // setup motor 1 to move backward 1.5 revolutions, this step does not 
//        // actually move the motor
//        //
//        stepper1.setupRelativeMoveInRevolutions(-1.5);
//
//        //
//        // setup motor 2 to move forward 3.0 revolutions, this step does not 
//        // actually move the motor
//        //
//        stepper2.setupRelativeMoveInRevolutions(3.0);
//
//        //
//        // execute the moves
//        //
//        while((!stepper1.motionComplete()) || (!stepper2.motionComplete()))
//        {
//          stepper1.processMovement();
//          stepper2.processMovement();
//        }
//

#include "SmallFlexyStepper.h"


//
// direction signal level for "step and direction"
//


// ---------------------------------------------------------------------------------
//                                  Setup functions 
// ---------------------------------------------------------------------------------


//
// constructor for the stepper class
//
SmallFlexyStepper::SmallFlexyStepper()
{
    //
    // initialize constants
    //
    stepsPerRevolution = 4076L;
    stepsPerMillimeter = 25.0;
    directionOfMotion = 0;
    currentPosition_InSteps = 0L;
    targetPosition_InSteps = 0L;
    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecondPerSecond(200.0);
    currentStepPeriod_InUS = 0.0;
    nextStepPeriod_InUS = 0.0;
}




//
// connect the stepper object to the IO pins
//  Enter:  stepPinNumber = IO pin number for the Step
//          directionPinNumber = IO pin number for the direction bit
//          enablePinNumber = IO pin number for the enable bit (LOW is enabled)
//            set to 0 if enable is not supported
//
void SmallFlexyStepper::connectToPins(byte inPin1, byte inPin2, byte inPin3, byte inPin4)
{
    //
    // remember the pin numbers
    //
    pin1 = inPin1;
    pin2 = inPin2;
    pin3 = inPin3;
    pin4 = inPin4;
    stepPhase = 0;

    //
    // configure the IO bits
    //
    pinMode(pin1, OUTPUT);
    digitalWrite(pin1, LOW);

    pinMode(pin2, OUTPUT);
    digitalWrite(pin2, LOW);

    pinMode(pin3, OUTPUT);
    digitalWrite(pin3, LOW);

    pinMode(pin4, OUTPUT);
    digitalWrite(pin4, LOW);
}



// ---------------------------------------------------------------------------------
//                     Public functions with units in millimeters 
// ---------------------------------------------------------------------------------

//
// set the number of steps the motor has per millimeters
//
void SmallFlexyStepper::setStepsPerMillimeter(float motorStepsPerMillimeter)
{
    stepsPerMillimeter = motorStepsPerMillimeter;
}



//
// get the current position of the motor in millimeters, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in millimeters returned
//
float SmallFlexyStepper::getCurrentPositionInMillimeters()
{
    return((float)getCurrentPositionInSteps() / stepsPerMillimeter);
}



//
// set the current position of the motor in millimeters, this does not move the 
// motor
//
void SmallFlexyStepper::setCurrentPositionInMillimeters(
    float currentPositionInMillimeters)
{
    setCurrentPositionInSteps((long)round(currentPositionInMillimeters *
        stepsPerMillimeter));
}



//
// set the maximum speed, units in millimeters/second, this is the maximum speed  
// reached while accelerating
//  Enter:  speedInMillimetersPerSecond = speed to accelerate up to, units in 
//            millimeters/second
//
void SmallFlexyStepper::setSpeedInMillimetersPerSecond(float speedInMillimetersPerSecond)
{
    setSpeedInStepsPerSecond(speedInMillimetersPerSecond * stepsPerMillimeter);
}



//
// set the rate of acceleration, units in millimeters/second/second
//  Enter:  accelerationInMillimetersPerSecondPerSecond = rate of acceleration,  
//          units in millimeters/second/second
//
void SmallFlexyStepper::setAccelerationInMillimetersPerSecondPerSecond(
    float accelerationInMillimetersPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(
        accelerationInMillimetersPerSecondPerSecond * stepsPerMillimeter);
}



//
// home the motor by moving until the homing sensor is activated, then set the  
// position to zero, with units in millimeters
//  Enter:  directionTowardHome = 1 to move in a positive direction, -1 to move 
//            in a negative directions 
//          speedInMillimetersPerSecond = speed to accelerate up to while moving 
//            toward home, units in millimeters/second
//          maxDistanceToMoveInMillimeters = unsigned maximum distance to move 
//            toward home before giving up
//          homeSwitchPin = pin number of the home switch, switch should be 
//            configured to go low when at home
//  Exit:   true returned if successful, else false
//
bool SmallFlexyStepper::moveToHomeInMillimeters(long directionTowardHome,
    float speedInMillimetersPerSecond, long maxDistanceToMoveInMillimeters,
    int homeLimitSwitchPin)
{
    return(moveToHomeInSteps(directionTowardHome,
        speedInMillimetersPerSecond * stepsPerMillimeter,
        maxDistanceToMoveInMillimeters * stepsPerMillimeter,
        homeLimitSwitchPin));
}



//
// move relative to the current position, units are in millimeters, this function  
// does not return until the move is complete
//  Enter:  distanceToMoveInMillimeters = signed distance to move relative to the  
//          current position in millimeters
//
void SmallFlexyStepper::moveRelativeInMillimeters(float distanceToMoveInMillimeters)
{
    setTargetPositionRelativeInMillimeters(distanceToMoveInMillimeters);

    while (!processMovement())
        ;
}



//
// setup a move relative to the current position, units are in millimeters, no   
// motion occurs until processMove() is called
//  Enter:  distanceToMoveInMillimeters = signed distance to move relative to the  
//          current position in millimeters
//
void SmallFlexyStepper::setTargetPositionRelativeInMillimeters(
    float distanceToMoveInMillimeters)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInMillimeters *
        stepsPerMillimeter));
}



//
// move to the given absolute position, units are in millimeters, this function 
// does not return until the move is complete
//  Enter:  absolutePositionToMoveToInMillimeters = signed absolute position to  
//          move to in units of millimeters
//
void SmallFlexyStepper::moveToPositionInMillimeters(
    float absolutePositionToMoveToInMillimeters)
{
    setTargetPositionInMillimeters(absolutePositionToMoveToInMillimeters);

    while (!processMovement())
        ;
}



//
// setup a move, units are in millimeters, no motion occurs until processMove() 
// is called
//  Enter:  absolutePositionToMoveToInMillimeters = signed absolute position to  
//          move to in units of millimeters
//
void SmallFlexyStepper::setTargetPositionInMillimeters(
    float absolutePositionToMoveToInMillimeters)
{
    setTargetPositionInSteps((long)round(absolutePositionToMoveToInMillimeters *
        stepsPerMillimeter));
}



//
// Get the current velocity of the motor in millimeters/second.  This functions is 
// updated while it accelerates up and down in speed.  This is not the desired  
// speed, but the speed the motor should be moving at the time the function is   
// called.  This is a signed value and is negative when the motor is moving 
// backwards.  Note: This speed will be incorrect if the desired velocity is set 
// faster than this library can generate steps, or if the load on the motor is too 
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float SmallFlexyStepper::getCurrentVelocityInMillimetersPerSecond()
{
    return(getCurrentVelocityInStepsPerSecond() / stepsPerMillimeter);
}



// ---------------------------------------------------------------------------------
//                     Public functions with units in revolutions 
// ---------------------------------------------------------------------------------

//
// set the number of steps the motor has per revolution
//
void SmallFlexyStepper::setStepsPerRevolution(float motorStepPerRevolution)
{
    stepsPerRevolution = motorStepPerRevolution;
}



//
// get the current position of the motor in revolutions, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in revolutions returned
//
float SmallFlexyStepper::getCurrentPositionInRevolutions()
{
    return((float)getCurrentPositionInSteps() / stepsPerRevolution);
}



//
// set the current position of the motor in revolutions, this does not move the 
// motor
//
void SmallFlexyStepper::setCurrentPositionInRevolutions(
    float currentPositionInRevolutions)
{
    setCurrentPositionInSteps((long)round(currentPositionInRevolutions *
        stepsPerRevolution));
}



//
// set the maximum speed, units in revolutions/second, this is the maximum speed  
// reached while accelerating
//  Enter:  speedInRevolutionsPerSecond = speed to accelerate up to, units in 
//            revolutions/second
//
void SmallFlexyStepper::setSpeedInRevolutionsPerSecond(float speedInRevolutionsPerSecond)
{
    setSpeedInStepsPerSecond(speedInRevolutionsPerSecond * stepsPerRevolution);
}



//
// set the rate of acceleration, units in revolutions/second/second
//  Enter:  accelerationInRevolutionsPerSecondPerSecond = rate of acceleration,  
//          units in revolutions/second/second
//
void SmallFlexyStepper::setAccelerationInRevolutionsPerSecondPerSecond(
    float accelerationInRevolutionsPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(
        accelerationInRevolutionsPerSecondPerSecond * stepsPerRevolution);
}



//
// home the motor by moving until the homing sensor is activated, then set the 
//  position to zero, with units in revolutions
//  Enter:  directionTowardHome = 1 to move in a positive direction, -1 to move in 
//            a negative directions 
//          speedInRevolutionsPerSecond = speed to accelerate up to while moving 
//            toward home, units in revolutions/second
//          maxDistanceToMoveInRevolutions = unsigned maximum distance to move 
//            toward home before giving up
//          homeSwitchPin = pin number of the home switch, switch should be 
//            configured to go low when at home
//  Exit:   true returned if successful, else false
//
bool SmallFlexyStepper::moveToHomeInRevolutions(long directionTowardHome,
    float speedInRevolutionsPerSecond, long maxDistanceToMoveInRevolutions,
    int homeLimitSwitchPin)
{
    return(moveToHomeInSteps(directionTowardHome,
        speedInRevolutionsPerSecond * stepsPerRevolution,
        maxDistanceToMoveInRevolutions * stepsPerRevolution,
        homeLimitSwitchPin));
}



//
// move relative to the current position, units are in revolutions, this function  
// does not return until the move is complete
//  Enter:  distanceToMoveInRevolutions = signed distance to move relative to the  
//          current position in revolutions
//
void SmallFlexyStepper::moveRelativeInRevolutions(float distanceToMoveInRevolutions)
{
    setTargetPositionRelativeInRevolutions(distanceToMoveInRevolutions);

    while (!processMovement())
        ;
}



//
// setup a move relative to the current position, units are in revolutions, no   
// motion occurs until processMove() is called
//  Enter:  distanceToMoveInRevolutions = signed distance to move relative to the  
//            currentposition in revolutions
//
void SmallFlexyStepper::setTargetPositionRelativeInRevolutions(
    float distanceToMoveInRevolutions)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInRevolutions *
        stepsPerRevolution));
}



//
// move to the given absolute position, units are in revolutions, this function 
// does not return until the move is complete
//  Enter:  absolutePositionToMoveToInRevolutions = signed absolute position to 
//            move to in units of revolutions
//
void SmallFlexyStepper::moveToPositionInRevolutions(
    float absolutePositionToMoveToInRevolutions)
{
    setTargetPositionInRevolutions(absolutePositionToMoveToInRevolutions);

    while (!processMovement())
        ;
}



//
// setup a move, units are in revolutions, no motion occurs until processMove() 
// is called
//  Enter:  absolutePositionToMoveToInRevolutions = signed absolute position to  
//          move to in units of revolutions
//
void SmallFlexyStepper::setTargetPositionInRevolutions(
    float absolutePositionToMoveToInRevolutions)
{
    setTargetPositionInSteps((long)round(absolutePositionToMoveToInRevolutions *
        stepsPerRevolution));
}



//
// Get the current velocity of the motor in revolutions/second.  This functions is 
// updated while it accelerates up and down in speed.  This is not the desired  
// speed, but the speed the motor should be moving at the time the function is   
// called.  This is a signed value and is negative when the motor is moving 
// backwards.  Note: This speed will be incorrect if the desired velocity is set 
// faster than this library can generate steps, or if the load on the motor is too 
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float SmallFlexyStepper::getCurrentVelocityInRevolutionsPerSecond()
{
    return(getCurrentVelocityInStepsPerSecond() / stepsPerRevolution);
}


// ---------------------------------------------------------------------------------
//                        Public functions with units in steps 
// ---------------------------------------------------------------------------------

//
// set the current position of the motor in steps, this does not move the motor
// Note: This function should only be called when the motor is stopped
//    Enter:  currentPositionInSteps = the new position of the motor in steps
//
void SmallFlexyStepper::setCurrentPositionInSteps(long currentPositionInSteps)
{
    currentPosition_InSteps = currentPositionInSteps;
}



//
// get the current position of the motor in steps, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in steps returned
//
long SmallFlexyStepper::getCurrentPositionInSteps()
{
    return(currentPosition_InSteps);
}



//
// set the maximum speed, units in steps/second, this is the maximum speed reached  
// while accelerating
//  Enter:  speedInStepsPerSecond = speed to accelerate up to, units in steps/second
//
void SmallFlexyStepper::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    desiredPeriod_InUSPerStep = 1000000.0 / desiredSpeed_InStepsPerSecond;
}



//
// set the rate of acceleration, units in steps/second/second
//  Enter:  accelerationInStepsPerSecondPerSecond = rate of acceleration, units in 
//          steps/second/second
//
void SmallFlexyStepper::setAccelerationInStepsPerSecondPerSecond(
    float accelerationInStepsPerSecondPerSecond)
{
    acceleration_InStepsPerSecondPerSecond = accelerationInStepsPerSecondPerSecond;
    acceleration_InStepsPerUSPerUS = acceleration_InStepsPerSecondPerSecond / 1E12;

    periodOfSlowestStep_InUS =
        1000000.0 / sqrt(2.0 * acceleration_InStepsPerSecondPerSecond);
    minimumPeriodForAStoppedMotion = periodOfSlowestStep_InUS / 2.8;
}



//
// home the motor by moving until the homing sensor is activated, then set the 
// position to zero with units in steps
//  Enter:  directionTowardHome = 1 to move in a positive direction, -1 to move in 
//            a negative directions 
//          speedInStepsPerSecond = speed to accelerate up to while moving toward 
//            home, units in steps/second
//          maxDistanceToMoveInSteps = unsigned maximum distance to move toward 
//            home before giving up
//          homeSwitchPin = pin number of the home switch, switch should be 
//            configured to go low when at home
//  Exit:   true returned if successful, else false
//
bool SmallFlexyStepper::moveToHomeInSteps(long directionTowardHome,
    float speedInStepsPerSecond, long maxDistanceToMoveInSteps,
    int homeLimitSwitchPin)
{
    float originalDesiredSpeed_InStepsPerSecond;
    bool limitSwitchFlag;


    //
    // setup the home switch input pin
    //
    pinMode(homeLimitSwitchPin, INPUT_PULLUP);


    //
    // remember the current speed setting
    //
    originalDesiredSpeed_InStepsPerSecond = desiredSpeed_InStepsPerSecond;


    //
    // if the home switch is not already set, move toward it
    //
    if (digitalRead(homeLimitSwitchPin) == HIGH)
    {
        //
        // move toward the home switch
        //
        setSpeedInStepsPerSecond(speedInStepsPerSecond);
        setTargetPositionRelativeInSteps(maxDistanceToMoveInSteps * directionTowardHome);
        limitSwitchFlag = false;
        while (!processMovement())
        {
            if (digitalRead(homeLimitSwitchPin) == LOW)
            {
                limitSwitchFlag = true;
                directionOfMotion = 0;
                break;
            }
        }

        //
        // check if switch never detected
        //
        if (limitSwitchFlag == false)
            return(false);
    }
    delay(25);


    //
    // the switch has been detected, now move away from the switch
    //
    setTargetPositionRelativeInSteps(maxDistanceToMoveInSteps *
        directionTowardHome * -1);
    limitSwitchFlag = false;
    while (!processMovement())
    {
        if (digitalRead(homeLimitSwitchPin) == HIGH)
        {
            limitSwitchFlag = true;
            directionOfMotion = 0;
            break;
        }
    }
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false)
        return(false);


    //
    // have now moved off the switch, move toward it again but slower
    //
    setSpeedInStepsPerSecond(speedInStepsPerSecond / 8);
    setTargetPositionRelativeInSteps(maxDistanceToMoveInSteps * directionTowardHome);
    limitSwitchFlag = false;
    while (!processMovement())
    {
        if (digitalRead(homeLimitSwitchPin) == LOW)
        {
            limitSwitchFlag = true;
            directionOfMotion = 0;
            break;
        }
    }
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false)
        return(false);


    //
    // successfully homed, set the current position to 0
    //
    setCurrentPositionInSteps(0L);

    //
    // restore original velocity
    //
    setSpeedInStepsPerSecond(originalDesiredSpeed_InStepsPerSecond);
    return(true);
}



//
// move relative to the current position, units are in steps, this function does 
// not return until the move is complete
//  Enter:  distanceToMoveInSteps = signed distance to move relative to the current  
//          position in steps
//
void SmallFlexyStepper::moveRelativeInSteps(long distanceToMoveInSteps)
{
    setTargetPositionRelativeInSteps(distanceToMoveInSteps);

    while (!processMovement())
        ;
}



//
// setup a move relative to the current position, units are in steps, no motion  
// occurs until processMove() is called
//  Enter:  distanceToMoveInSteps = signed distance to move relative to the current  
//            positionin steps
//
void SmallFlexyStepper::setTargetPositionRelativeInSteps(long distanceToMoveInSteps)
{
    setTargetPositionInSteps(currentPosition_InSteps + distanceToMoveInSteps);
}



//
// move to the given absolute position, units are in steps, this function does not 
// return until the move is complete
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to  
//            in unitsof steps
//
void SmallFlexyStepper::moveToPositionInSteps(long absolutePositionToMoveToInSteps)
{
    setTargetPositionInSteps(absolutePositionToMoveToInSteps);

    while (!processMovement())
        ;
}



//
// setup a move, units are in steps, no motion occurs until processMove() is called
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to  
//            in units of steps
//
void SmallFlexyStepper::setTargetPositionInSteps(long absolutePositionToMoveToInSteps)
{
    targetPosition_InSteps = absolutePositionToMoveToInSteps;
}



//
// setup a "Stop" to begin the process of decelerating from the current velocity  
// to zero, decelerating requires calls to processMove() until the move is complete
// Note: This function can be used to stop a motion initiated in units of steps 
// or revolutions
//
void SmallFlexyStepper::setTargetPositionToStop()
{
    long decelerationDistance_InSteps;

    //
    // move the target position so that the motor will begin deceleration now
    //
    decelerationDistance_InSteps = (long)round(
        5E11 / (acceleration_InStepsPerSecondPerSecond * currentStepPeriod_InUS *
            currentStepPeriod_InUS));

    if (directionOfMotion > 0)
        setTargetPositionInSteps(currentPosition_InSteps + decelerationDistance_InSteps);
    else
        setTargetPositionInSteps(currentPosition_InSteps - decelerationDistance_InSteps);
}

void SmallFlexyStepper::deadStop()
{
    directionOfMotion = 0;
    targetPosition_InSteps = currentPosition_InSteps;
}

//
// if it is time, move one step
//  Exit:  true returned if movement complete, false returned not a final target 
//           position yet
//
bool SmallFlexyStepper::processMovement(void)
{
    unsigned long currentTime_InUS;
    unsigned long periodSinceLastStep_InUS;
    long distanceToTarget_Signed;


    //
    // check if currently stopped
    //
    if (directionOfMotion == 0)
    {
        distanceToTarget_Signed = targetPosition_InSteps - currentPosition_InSteps;

        //
        // check if target position in a positive direction
        //
        if (distanceToTarget_Signed > 0)
        {
            directionOfMotion = 1;
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
            lastStepTime_InUS = micros();
            return(false);
        }

        //
        // check if target position in a negative direction
        //
        else if (distanceToTarget_Signed < 0)
        {
            directionOfMotion = -1;
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
            lastStepTime_InUS = micros();
            return(false);
        }

        else
            return(true);
    }


    //
    // determine how much time has elapsed since the last step (Note 1: this method   
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    //
    currentTime_InUS = micros();
    periodSinceLastStep_InUS = currentTime_InUS - lastStepTime_InUS;


    //
    // if it is not time for the next step, return
    //
    if (periodSinceLastStep_InUS < (unsigned long)nextStepPeriod_InUS)
        return(false);


    //
    // execute the step on the rising edge
    //
    setNextFullStep();


    //
    // update the current position and speed
    //
    currentPosition_InSteps += directionOfMotion;
    currentStepPeriod_InUS = nextStepPeriod_InUS;


    //
    // remember the time that this step occured
    //
    lastStepTime_InUS = currentTime_InUS;


    //
    // figure out how long before the next step
    //
    DeterminePeriodOfNextStep();



    //
    // check if the move has reached its final target position, return true if all 
    // done
    //
    if (currentPosition_InSteps == targetPosition_InSteps)
    {
        //
        // at final position, make sure the motor is not going too fast
        //
        if (nextStepPeriod_InUS >= minimumPeriodForAStoppedMotion)
        {
            currentStepPeriod_InUS = 0.0;
            nextStepPeriod_InUS = 0.0;
            directionOfMotion = 0;
            return(true);
        }
    }

    return(false);
}

void SmallFlexyStepper::setNextFullStep()
{
    //
    // compute the next phase number
    //
    stepPhase += (-directionOfMotion);

    if (stepPhase <= -1)
        stepPhase = 3;

    if (stepPhase >= 4)
        stepPhase = 0;

    //
    // set the coils for this phase
    //
    switch (stepPhase)
    {
    case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        digitalWrite(pin4, HIGH);
        break;
    case 1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, HIGH);
        digitalWrite(pin4, LOW);
        break;
    case 2:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        break;
    case 3:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, HIGH);
        break;
    }
}


//
// Get the current velocity of the motor in steps/second.  This functions is 
// updated while it accelerates up and down in speed.  This is not the desired  
// speed, but the speed the motor should be moving at the time the function is  
// called.  This is a signed value and is negative when the motor is moving 
// backwards.  Note: This speed will be incorrect if the desired velocity is set 
// faster than this library can generate steps, or if the load on the motor is too 
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float SmallFlexyStepper::getCurrentVelocityInStepsPerSecond()
{
    if (currentStepPeriod_InUS == 0.0)
        return(0);
    else
    {
        if (directionOfMotion > 0)
            return(1000000.0 / currentStepPeriod_InUS);
        else
            return(-1000000.0 / currentStepPeriod_InUS);
    }
}



//
// check if the motor has competed its move to the target position
//  Exit:  true returned if the stepper is at the target position
//
bool SmallFlexyStepper::motionComplete()
{
    if ((directionOfMotion == 0) &&
        (currentPosition_InSteps == targetPosition_InSteps))
        return(true);
    else
        return(false);
}



//
// determine the period for the next step, either speed up a little, slow down a  
// little or go the same speed
//
void SmallFlexyStepper::DeterminePeriodOfNextStep()
{
    long distanceToTarget_Signed;
    long distanceToTarget_Unsigned;
    float currentSpeed_InStepsPerSecond;
    long decelerationDistance_InSteps;
    float currentStepPeriodSquared;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;


    //
    // determine the distance to the target position
    //
    distanceToTarget_Signed = targetPosition_InSteps - currentPosition_InSteps;
    if (distanceToTarget_Signed >= 0L)
    {
        distanceToTarget_Unsigned = distanceToTarget_Signed;
        targetInPositiveDirectionFlag = true;
    }
    else
    {
        distanceToTarget_Unsigned = -distanceToTarget_Signed;
        targetInNegativeDirectionFlag = true;
    }


    //
    // determine the number of steps needed to go from the current speed down to a 
    // velocity of 0, Steps = Velocity^2 / (2 * Acceleration)
    //
    currentStepPeriodSquared = currentStepPeriod_InUS * currentStepPeriod_InUS;
    decelerationDistance_InSteps = (long)round(
        5E11 / (acceleration_InStepsPerSecondPerSecond * currentStepPeriodSquared));


    //
    // check if: Moving in a positive direction & Moving toward the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed > 0)
    //
    if ((directionOfMotion == 1) && (targetInPositiveDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we 
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (nextStepPeriod_InUS < desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }


    //
    // check if: Moving in a positive direction & Moving away from the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed < 0)
    //
    else if ((directionOfMotion == 1) && (targetInNegativeDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (currentStepPeriod_InUS < periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            directionOfMotion = -1;
        }
    }


    //
    // check if: Moving in a negative direction & Moving toward the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed < 0)
    //
    else if ((directionOfMotion == -1) && (targetInNegativeDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we 
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (nextStepPeriod_InUS < desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }


    //
    // check if: Moving in a negative direction & Moving away from the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed > 0)
    //
    else if ((directionOfMotion == -1) && (targetInPositiveDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (currentStepPeriod_InUS < periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            directionOfMotion = 1;
        }
    }


    //
    // check if accelerating
    //
    if (speedUpFlag)
    {
        //
        // StepPeriod = StepPeriod(1 - a * StepPeriod^2)
        //
        nextStepPeriod_InUS = currentStepPeriod_InUS - acceleration_InStepsPerUSPerUS *
            currentStepPeriodSquared * currentStepPeriod_InUS;

        if (nextStepPeriod_InUS < desiredPeriod_InUSPerStep)
            nextStepPeriod_InUS = desiredPeriod_InUSPerStep;
    }


    //
    // check if decelerating
    //
    if (slowDownFlag)
    {
        //
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        //
        nextStepPeriod_InUS = currentStepPeriod_InUS + acceleration_InStepsPerUSPerUS *
            currentStepPeriodSquared * currentStepPeriod_InUS;

        if (nextStepPeriod_InUS > periodOfSlowestStep_InUS)
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
    }
}

//
// disable the motor, all the drive coils are turned off to save power
// and reduce heat when motor is not in motion, any movement command will
// automatically renable the stepper
//
void SmallFlexyStepper::disable()
{
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
}

// -------------------------------------- End --------------------------------------
