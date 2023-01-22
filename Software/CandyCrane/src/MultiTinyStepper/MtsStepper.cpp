#include "MtsStepper.h"

MtsStepper::MtsStepper()
{
    _stepSize = MTS_STEPPER_FULL_STEP;
    _stepsPerRevolution = MTS_STEPPER_TYPE_16_REV;
    _stepsPerMillimeter = 25.0;
    _directionOfMotion = 0;
    _currentPosition_InSteps = 0L;
    _targetPosition_InSteps = 0L;
    setSpeedInStepsPerSecond(200);
    setAccelerationInStepsPerSecondPerSecond(200.0);
    _currentStepPeriod_InUS = 0.0;
    _nextStepPeriod_InUS = 0.0;
}

void MtsStepper::setStepperType(MTS_STEPPER_TYPE type, MTS_STEPPER_STEP_SIZE stepSize)
{
    _stepperType = type;
    _stepSize = stepSize;

    if (_stepperType == MTS_STEPPER_TYPE_64) {
        if (_stepSize == MTS_STEPPER_FULL_STEP) {
            _stepsPerRevolution = MTS_STEPPER_TYPE_64_REV;
        }
        else if (_stepSize == MTS_STEPPER_HALF_STEP) {
            _stepsPerRevolution = MTS_STEPPER_TYPE_64_REV * 2;
        }
    }
    else if (_stepperType == MTS_STEPPER_TYPE_16) {
        if (_stepSize == MTS_STEPPER_FULL_STEP) {
            _stepsPerRevolution = MTS_STEPPER_TYPE_16_REV;
        }
        else if (_stepSize == MTS_STEPPER_HALF_STEP) {
            _stepsPerRevolution = MTS_STEPPER_TYPE_16_REV * 2;
        }
    }
}


// Steps

void MtsStepper::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    _desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    _desiredPeriod_InUSPerStep = 1000000.0 / _desiredSpeed_InStepsPerSecond;
}

void MtsStepper::setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond)
{
    _acceleration_InStepsPerSecondPerSecond = accelerationInStepsPerSecondPerSecond;
    _acceleration_InStepsPerUSPerUS = _acceleration_InStepsPerSecondPerSecond / 1E12;

    _periodOfSlowestStep_InUS = 1000000.0 / sqrt(2.0 * _acceleration_InStepsPerSecondPerSecond);
    _minimumPeriodForAStoppedMotion = _periodOfSlowestStep_InUS / 2.8;
}

void MtsStepper::setCurrentPositionInSteps(long currentPositionInSteps)
{
    _currentPosition_InSteps = currentPositionInSteps;
}

long MtsStepper::getCurrentPositionInSteps()
{
    return(_currentPosition_InSteps);
}

void MtsStepper::setTargetPositionRelativeInSteps(long distanceToMoveInSteps)
{
    setTargetPositionInSteps(_currentPosition_InSteps + distanceToMoveInSteps);
}

void MtsStepper::setTargetPositionInSteps(long absolutePositionToMoveToInSteps)
{
    _targetPosition_InSteps = absolutePositionToMoveToInSteps;
}

float MtsStepper::getCurrentVelocityInStepsPerSecond()
{
    if (_currentStepPeriod_InUS == 0.0) {
        return(0);
    }   
    else {
        if (_directionOfMotion > 0) {
            return(1000000.0 / _currentStepPeriod_InUS);
        }
        else {
            return(-1000000.0 / _currentStepPeriod_InUS);
        }
    }
}



// Millimeters

void MtsStepper::setStepsPerMillimeter(float motorStepsPerMillimeter)
{
    _stepsPerMillimeter = motorStepsPerMillimeter;
}

float MtsStepper::getCurrentPositionInMillimeters()
{
    return((float)getCurrentPositionInSteps() / _stepsPerMillimeter);
}


void MtsStepper::setCurrentPositionInMillimeters(float currentPositionInMillimeters)
{
    setCurrentPositionInSteps((long)round(currentPositionInMillimeters * _stepsPerMillimeter));
}

void MtsStepper::setSpeedInMillimetersPerSecond(float speedInMillimetersPerSecond)
{
    setSpeedInStepsPerSecond(speedInMillimetersPerSecond * _stepsPerMillimeter);
}

void MtsStepper::setAccelerationInMillimetersPerSecondPerSecond(
    float accelerationInMillimetersPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(accelerationInMillimetersPerSecondPerSecond * _stepsPerMillimeter);
}

bool MtsStepper::moveToHomeInMillimeters(long directionTowardHome,float speedInMillimetersPerSecond, long maxDistanceToMoveInMillimeters,int homeLimitSwitchPin)
{
    return(moveToHomeInSteps(directionTowardHome,
        speedInMillimetersPerSecond * _stepsPerMillimeter,
        maxDistanceToMoveInMillimeters * _stepsPerMillimeter,
        homeLimitSwitchPin));
}

void MtsStepper::setTargetPositionRelativeInMillimeters(float distanceToMoveInMillimeters)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInMillimeters * _stepsPerMillimeter));
}

void MtsStepper::setTargetPositionInMillimeters(float absolutePositionToMoveToInMillimeters)
{
    setTargetPositionInSteps((long)round(absolutePositionToMoveToInMillimeters * _stepsPerMillimeter));
}

float MtsStepper::getCurrentVelocityInMillimetersPerSecond()
{
    return(getCurrentVelocityInStepsPerSecond() / _stepsPerMillimeter);
}



// Revolutions

void MtsStepper::setStepsPerRevolution(float motorStepPerRevolution)
{
    _stepsPerRevolution = motorStepPerRevolution;
}

void MtsStepper::setCurrentPositionInRevolutions(float currentPositionInRevolutions)
{
    setCurrentPositionInSteps((long)round(currentPositionInRevolutions * _stepsPerRevolution));
}

void MtsStepper::setSpeedInRevolutionsPerSecond(float speedInRevolutionsPerSecond)
{
    setSpeedInStepsPerSecond(speedInRevolutionsPerSecond * _stepsPerRevolution);
}

void MtsStepper::setAccelerationInRevolutionsPerSecondPerSecond(float accelerationInRevolutionsPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(accelerationInRevolutionsPerSecondPerSecond * _stepsPerRevolution);
}

bool MtsStepper::moveToHomeInRevolutions(long directionTowardHome, float speedInRevolutionsPerSecond, long maxDistanceToMoveInRevolutions, int homeLimitSwitchPin)
{
    return(moveToHomeInSteps(directionTowardHome,
        speedInRevolutionsPerSecond * _stepsPerRevolution,
        maxDistanceToMoveInRevolutions * _stepsPerRevolution,
        homeLimitSwitchPin));
}

void MtsStepper::setTargetPositionRelativeInRevolutions(float distanceToMoveInRevolutions)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInRevolutions * _stepsPerRevolution));
}

void MtsStepper::setTargetPositionInRevolutions(float absolutePositionToMoveToInRevolutions)
{
    long val = (long)round(absolutePositionToMoveToInRevolutions * _stepsPerRevolution);
    setTargetPositionInSteps(val);
}

float MtsStepper::getCurrentVelocityInRevolutionsPerSecond()
{
    return(getCurrentVelocityInStepsPerSecond() / _stepsPerRevolution);
}

float MtsStepper::getCurrentPositionInRevolutions()
{
    return((float)getCurrentPositionInSteps() / _stepsPerRevolution);
}



// General

bool MtsStepper::motionComplete()
{
    if ((_directionOfMotion == 0) && (_currentPosition_InSteps == _targetPosition_InSteps)) {
        return(true);
    }

    return(false);
}

void MtsStepper::deadStop()
{
    _directionOfMotion = 0;
    _targetPosition_InSteps = _currentPosition_InSteps;
}

void MtsStepper::disable()
{
    deadStop();
    processStep(-1, _stepperType);
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
bool MtsStepper::moveToHomeInSteps(long directionTowardHome,
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
    originalDesiredSpeed_InStepsPerSecond = _desiredSpeed_InStepsPerSecond;

    //
    // if the home switch is not already set, move toward it
    //
    if (digitalRead(homeLimitSwitchPin) == HIGH) {
        //
        // move toward the home switch
        //
        setSpeedInStepsPerSecond(speedInStepsPerSecond);
        setTargetPositionRelativeInSteps(maxDistanceToMoveInSteps * directionTowardHome);
        limitSwitchFlag = false;
        while (!process()) {
            if (digitalRead(homeLimitSwitchPin) == LOW) {
                limitSwitchFlag = true;
                _directionOfMotion = 0;
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
    while (!process())
    {
        if (digitalRead(homeLimitSwitchPin) == HIGH)
        {
            limitSwitchFlag = true;
            _directionOfMotion = 0;
            break;
        }
    }
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false) {
        return(false);
    }

    //
    // have now moved off the switch, move toward it again but slower
    //
    setSpeedInStepsPerSecond(speedInStepsPerSecond / 8);
    setTargetPositionRelativeInSteps(maxDistanceToMoveInSteps * directionTowardHome);
    limitSwitchFlag = false;
    while (!process()) {
        if (digitalRead(homeLimitSwitchPin) == LOW) {
            limitSwitchFlag = true;
            _directionOfMotion = 0;
            break;
        }
    }
    delay(25);

    //
    // check if switch never detected
    //
    if (limitSwitchFlag == false) {
        return(false);
    }

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



bool MtsStepper::process()
{
    unsigned long currentTime_InUS;
    unsigned long periodSinceLastStep_InUS;
    long distanceToTarget_Signed;

    //
    // check if currently stopped
    //
    if (_directionOfMotion == 0) {
        distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;

        //
        // check if target position in a positive direction
        //
        if (distanceToTarget_Signed > 0) {
            _directionOfMotion = 1;
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = 0;
        }

        //
        // check if target position in a negative direction
        //
        else if (distanceToTarget_Signed < 0) {
            _directionOfMotion = -1;
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
            _lastStepTime_InUS = 0;
        }

        else {
            return(true);
        }   
    }

    //
    // determine how much time has elapsed since the last step (Note 1: this method   
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    //
    currentTime_InUS = micros();
    periodSinceLastStep_InUS = currentTime_InUS - _lastStepTime_InUS;

    //
    // if it is not time for the next step, return
    //
    if (periodSinceLastStep_InUS < (unsigned long)_nextStepPeriod_InUS) {
        return(false);
    }
        

    //
    // execute the step
    //
    setNextStep();

    //
    // update the current position and speed
    //
    _currentPosition_InSteps += _directionOfMotion;
    _currentStepPeriod_InUS = _nextStepPeriod_InUS;

    //
    // remember the time that this step occured
    //
    _lastStepTime_InUS = currentTime_InUS;

    //
    // figure out how long before the next step
    //
    DeterminePeriodOfNextStep();

    //
    // check if the move has reached its final target position, return true if all 
    // done
    //
    if (_currentPosition_InSteps == _targetPosition_InSteps) {
        //
        // at final position, make sure the motor is not going too fast
        //
        if (_nextStepPeriod_InUS >= _minimumPeriodForAStoppedMotion) {
            _currentStepPeriod_InUS = 0.0;
            _nextStepPeriod_InUS = 0.0;
            _directionOfMotion = 0;
            return(true);
        }
    }

    return(false);
}



void MtsStepper::setNextStep()
{
    _stepPhase += (-_directionOfMotion);

    if (_stepSize == MTS_STEPPER_FULL_STEP) {
        if (_stepPhase <= -1) {
            _stepPhase = 3;
        }
        if (_stepPhase >= 4) {
            _stepPhase = 0;
        }   
    } 
    else if (_stepSize == MTS_STEPPER_HALF_STEP) {
        if (_stepPhase <= -1) {
            _stepPhase = 7;
        }
        if (_stepPhase >= 8) {
            _stepPhase = 0;
        }
    }

    processStep(_stepPhase, _stepSize);
}

void MtsStepper::DeterminePeriodOfNextStep()
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
    distanceToTarget_Signed = _targetPosition_InSteps - _currentPosition_InSteps;
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
    currentStepPeriodSquared = _currentStepPeriod_InUS * _currentStepPeriod_InUS;
    decelerationDistance_InSteps = (long)round(
        5E11 / (_acceleration_InStepsPerSecondPerSecond * currentStepPeriodSquared));


    //
    // check if: Moving in a positive direction & Moving toward the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed > 0)
    //
    if ((_directionOfMotion == 1) && (targetInPositiveDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we 
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }


    //
    // check if: Moving in a positive direction & Moving away from the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed < 0)
    //
    else if ((_directionOfMotion == 1) && (targetInNegativeDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            _directionOfMotion = -1;
        }
    }


    //
    // check if: Moving in a negative direction & Moving toward the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed < 0)
    //
    else if ((_directionOfMotion == -1) && (targetInNegativeDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we 
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }


    //
    // check if: Moving in a negative direction & Moving away from the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed > 0)
    //
    else if ((_directionOfMotion == -1) && (targetInPositiveDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (_currentStepPeriod_InUS < _periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            _directionOfMotion = 1;
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
        _nextStepPeriod_InUS = _currentStepPeriod_InUS - _acceleration_InStepsPerUSPerUS *
            currentStepPeriodSquared * _currentStepPeriod_InUS;

        if (_nextStepPeriod_InUS < _desiredPeriod_InUSPerStep)
            _nextStepPeriod_InUS = _desiredPeriod_InUSPerStep;
    }


    //
    // check if decelerating
    //
    if (slowDownFlag)
    {
        //
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        //
        _nextStepPeriod_InUS = _currentStepPeriod_InUS + _acceleration_InStepsPerUSPerUS *
            currentStepPeriodSquared * _currentStepPeriod_InUS;

        if (_nextStepPeriod_InUS > _periodOfSlowestStep_InUS)
            _nextStepPeriod_InUS = _periodOfSlowestStep_InUS;
    }
}