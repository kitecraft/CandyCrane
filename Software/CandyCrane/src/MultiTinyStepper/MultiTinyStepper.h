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

	uint8_t _portA = 0;
	uint8_t _portB = 0;

	uint8_t _lowerMask = 0X0F;
	uint8_t _upperMask = 0XF0;

	void processStepper1(uint8_t phase, uint8_t stepType);
	void processStepper2(uint8_t phase, uint8_t stepType);
	void processStepper3(uint8_t phase, uint8_t stepType);
	void processStepper4(uint8_t phase, uint8_t stepType);

	uint8_t getFullStepForPhase(uint8_t phase);
	uint8_t getHalfStepForPhase(uint8_t phase);

	portMUX_TYPE _mutex;
public:
	MultiTinyStepper() {}
	void begin(TwoWire& wire, int address = MCP23017_DEFAULT_ADDR);

	/*
	* Get a handle to a stepper.
	* Do not delete it.
	*/
	MtsStepper* getStepper(MTS_STEPPER_ID stepperId);
};

