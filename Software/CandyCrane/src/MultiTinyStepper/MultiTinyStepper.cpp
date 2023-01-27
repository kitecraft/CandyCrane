#include "MultiTinyStepper.h"

void MultiTinyStepper::begin(TwoWire& wire, int8_t resetPin, int address)
{
	_mutex = portMUX_INITIALIZER_UNLOCKED;
	_resetPin = resetPin;
	_address = address;
	_wire = &wire;
	pinMode(_resetPin, OUTPUT);
	Reset();


	_steppers[MTS_STEPPER_1].setCallback([this](uint8_t data, uint8_t stepType, bool reversed) {
		processStepper1(data, stepType, reversed);
	});
	_steppers[MTS_STEPPER_2].setCallback([this](uint8_t data, uint8_t stepType, bool reversed) {
		processStepper2(data, stepType, reversed);
		});
	_steppers[MTS_STEPPER_3].setCallback([this](uint8_t data, uint8_t stepType, bool reversed) {
		processStepper3(data, stepType, reversed);
		});
	_steppers[MTS_STEPPER_4].setCallback([this](uint8_t data, uint8_t stepType, bool reversed) {
		processStepper4(data, stepType, reversed);
		});
}

void MultiTinyStepper::Reset()
{
	digitalWrite(_resetPin, LOW);
	delay(200);
	digitalWrite(_resetPin, HIGH);

	_mcp.init(_address, *_wire);
	_mcp.portMode(MCP23017Port::A, 0);
	_mcp.portMode(MCP23017Port::B, 0);

	_portA = 0;
	_portB = 0;
	_mcp.writePort(MCP23017Port::A, _portA);
	_mcp.writePort(MCP23017Port::B, _portB);
}


MtsStepper* MultiTinyStepper::getStepper(MTS_STEPPER_ID stepperId)
{
	if (stepperId < 0 || stepperId > 3) {
		return nullptr;
	}
	return &_steppers[stepperId];
}

void MultiTinyStepper::processStepper1(uint8_t phase, uint8_t stepType, bool reversed)
{
	uint16_t step = 0;
	if (stepType == MTS_STEPPER_FULL_STEP) {
		step = getFullStepForPhase(phase, reversed);
	}
	else if (stepType == MTS_STEPPER_HALF_STEP) {
		step = getHalfStepForPhase(phase, reversed);
	}

	portENTER_CRITICAL(&_mutex);
	_portB = (_portB & _upperMask) | step;
	uint16_t copy = _portB;
	portEXIT_CRITICAL(&_mutex);

	_mcp.writePort(MCP23017Port::B, copy);
}

void MultiTinyStepper::processStepper2(uint8_t phase, uint8_t stepType, bool reversed)
{
	uint16_t step = 0;
	if (stepType == MTS_STEPPER_FULL_STEP) {
		step = getFullStepForPhase(phase, reversed) << 4;
	}
	else if (stepType == MTS_STEPPER_HALF_STEP) {
		step = getHalfStepForPhase(phase, reversed) << 4;
	}

	portENTER_CRITICAL(&_mutex);
	_portB = (_portB & _lowerMask) | step;
	uint16_t copy = _portB;
	portEXIT_CRITICAL(&_mutex);

	_mcp.writePort(MCP23017Port::B, copy);
}

void MultiTinyStepper::processStepper3(uint8_t phase, uint8_t stepType, bool reversed)
{
	uint16_t step = 0;
	if (stepType == MTS_STEPPER_FULL_STEP) {
		step = getFullStepForPhase(phase, reversed);
	}
	else if (stepType == MTS_STEPPER_HALF_STEP) {
		step = getHalfStepForPhase(phase, reversed);
	}

	portENTER_CRITICAL(&_mutex);
	_portA = (_portA & _upperMask) | step;
	uint16_t copy = _portA;
	portEXIT_CRITICAL(&_mutex);

	_mcp.writePort(MCP23017Port::A, copy);
}

void MultiTinyStepper::processStepper4(uint8_t phase, uint8_t stepType, bool reversed)
{
	uint16_t step = 0;
	if (stepType == MTS_STEPPER_FULL_STEP) {
		step = getFullStepForPhase(phase, reversed) << 4;
	}
	else if (stepType == MTS_STEPPER_HALF_STEP) {
		step = getHalfStepForPhase(phase, reversed) << 4;
	}

	portENTER_CRITICAL(&_mutex);
	_portA = (_portA & _lowerMask) | step;
	uint16_t copy = _portA;
	portEXIT_CRITICAL(&_mutex);

	_mcp.writePort(MCP23017Port::A, copy);
}

uint8_t MultiTinyStepper::getFullStepForPhase(uint8_t phase, bool reversed)
{
	if (reversed) {
		return getFullStepReversed(phase);
	}

	return getFullStepForward(phase);
}

uint8_t MultiTinyStepper::getFullStepForward(uint8_t phase)
{
	uint8_t ret = 0;

	switch (phase) {
	case MTS_STEPPER_PHASE_DISABLE:
		ret = 0b0000;
		break;
	case MTS_STEPPER_PHASE_1:
		ret = 0b1100;
		break;
	case MTS_STEPPER_PHASE_2:
		ret = 0b0110;
		break;
	case MTS_STEPPER_PHASE_3:
		ret = 0b0011;
		break;
	case MTS_STEPPER_PHASE_4:
		ret = 0b1001;
		break;
	}

	return ret;
}

uint8_t MultiTinyStepper::getFullStepReversed(uint8_t phase)
{
	uint8_t ret = 0;

	switch (phase) {
	case MTS_STEPPER_PHASE_DISABLE:
		ret = 0b0000;
		break;
	case MTS_STEPPER_PHASE_1:
		ret = 0b1001;
		break;
	case MTS_STEPPER_PHASE_2:
		ret = 0b0011;
		break;
	case MTS_STEPPER_PHASE_3:
		ret = 0b0110;
		break;
	case MTS_STEPPER_PHASE_4:
		ret = 0b1100;
		break;
	}

	return ret;
}


uint8_t MultiTinyStepper::getHalfStepForPhase(uint8_t phase, bool reversed)
{
	if (reversed) {
		return getHalfStepReversed(phase);
	}

	return getHalfStepForward(phase);
}

uint8_t MultiTinyStepper::getHalfStepForward(uint8_t phase)
{
	uint8_t ret = 0;

	switch (phase) {
	case MTS_STEPPER_PHASE_DISABLE:
		ret = 0b0000;
		break;
	case MTS_STEPPER_PHASE_1:
		ret = 0b1100;
		break;
	case MTS_STEPPER_PHASE_2:
		ret = 0b0100;
		break;
	case MTS_STEPPER_PHASE_3:
		ret = 0b0110;
		break;
	case MTS_STEPPER_PHASE_4:
		ret = 0b0010;
		break;
	case MTS_STEPPER_PHASE_5:
		ret = 0b0011;
		break;
	case MTS_STEPPER_PHASE_6:
		ret = 0b0001;
		break;
	case MTS_STEPPER_PHASE_7:
		ret = 0b1001;
		break;
	case MTS_STEPPER_PHASE_8:
		ret = 0b1000;
		break;
	}
	return ret;
}

uint8_t MultiTinyStepper::getHalfStepReversed(uint8_t phase)
{
	uint8_t ret = 0;

	switch (phase) {
	case MTS_STEPPER_PHASE_DISABLE:
		ret = 0b0000;
		break;
	case MTS_STEPPER_PHASE_1:
		ret = 0b1000;
		break;
	case MTS_STEPPER_PHASE_2:
		ret = 0b1001;
		break;
	case MTS_STEPPER_PHASE_3:
		ret = 0b0001;
		break;
	case MTS_STEPPER_PHASE_4:
		ret = 0b0011;
		break;
	case MTS_STEPPER_PHASE_5:
		ret = 0b0010;
		break;
	case MTS_STEPPER_PHASE_6:
		ret = 0b0110;
		break;
	case MTS_STEPPER_PHASE_7:
		ret = 0b0100;
		break;
	case MTS_STEPPER_PHASE_8:
		ret = 0b1100;
		break;
	}
	return ret;

}