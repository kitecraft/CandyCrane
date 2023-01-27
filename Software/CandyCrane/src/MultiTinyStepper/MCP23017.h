#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "MultiTinyStepper_Config.h"

class MCP23017
{
private:
	TwoWire* _bus;
	uint8_t _deviceAddr;
	
public:
	MCP23017() {}
	void init(uint8_t address, TwoWire& bus = Wire);
	void portMode(MCP23017Port port, uint8_t directions, uint8_t pullups = 0xFF, uint8_t inverted = 0x00);
	void writePort(MCP23017Port port, uint8_t value);
	void writeRegister(MCP23017Register reg, uint8_t value);
	void writeRegister(MCP23017Register reg, uint8_t portA, uint8_t portB);

};