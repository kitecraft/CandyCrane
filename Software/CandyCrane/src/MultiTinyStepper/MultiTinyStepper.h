#pragma once

// This stepper motor driver is based on the original FlexyStepper project by S. Reifel & Co 
// https://github.com/Stan-Reifel/FlexyStepper

#include <Arduino.h>
#include <Wire.h>
#include "MCP23017.h"
#include "MultiTinyStepper_Config.h"
#include "MtsStepper.h"


class MultiTinyStepper
{
private:
	MCP23017 _mcp;
	MtsStepper _steppers[MTS_STEPPER_COUNT];
	bool _stepperInUse[MTS_STEPPER_COUNT] = { false };
	int _address = 0;
	int8_t _resetPin = -1;
	TwoWire* _wire;

	bool _globalControl = false;

	uint8_t _portA = 0;
	uint8_t _portB = 0;

	uint8_t _lowerMask = 0X0F;
	uint8_t _upperMask = 0XF0;

	void processStepper1(uint8_t phase, uint8_t stepType, bool reversed);
	void processStepper2(uint8_t phase, uint8_t stepType, bool reversed);
	void processStepper3(uint8_t phase, uint8_t stepType, bool reversed);
	void processStepper4(uint8_t phase, uint8_t stepType, bool reversed);

	uint8_t getFullStepForward(uint8_t phase);
	uint8_t getFullStepReversed(uint8_t phase);
	uint8_t getHalfStepForward(uint8_t phase);
	uint8_t getHalfStepReversed(uint8_t phase);

	uint8_t getFullStepForPhase(uint8_t phase, bool reversed);
	uint8_t getHalfStepForPhase(uint8_t phase, bool reversed);

	portMUX_TYPE _mutex;
public:
	MultiTinyStepper() {}
	void begin(TwoWire& wire, int8_t resetPin = -1, int address = MCP23017_DEFAULT_ADDR);
	void Reset();

	/*
	* Get a handle to a stepper.
	* Do not delete it.
	*/
	MtsStepper* getStepper(MTS_STEPPER_ID stepperId);

	/*
	* When global control is on, you must all processAll()
	* When off, you must call each steepers process()
	*/
	void setGlobalControl(bool global) { _globalControl = global; }

	/*
	* Process All stepper motors simultaneously
	*/
	bool processAll();
};

