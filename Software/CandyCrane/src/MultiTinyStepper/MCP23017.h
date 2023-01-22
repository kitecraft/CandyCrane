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
	/**
	 * Initializes the chip with the default configuration.
	 * Enables Byte mode (IOCON.BANK = 0 and IOCON.SEQOP = 1).
	 * Enables pull-up resistors for all pins. This will only be effective for input pins.
	 * 
	 * See "3.2.1 Byte mode and Sequential mode".
	 */
	void init(uint8_t address, TwoWire& bus = Wire);

	/**
	 * Controls the pins direction on a whole port at once.
	 * 
	 * 1 = Pin is configured as an input.
	 * 0 = Pin is configured as an output.
	 * 
	 * See "3.5.1 I/O Direction register".
	 */
	void portMode(MCP23017Port port, uint8_t directions, uint8_t pullups = 0xFF, uint8_t inverted = 0x00);


	/**
	 * Writes pins state to a whole port.
	 * 
	 * 1 = Logic-high
	 * 0 = Logic-low
	 * 
	 * See "3.5.10 Port register".
	 */
	void writePort(MCP23017Port port, uint8_t value);

	/**
	 * Writes a single register value.
	 */
	void writeRegister(MCP23017Register reg, uint8_t value);
	/**
	 * Writes values to a register pair.
	 * 
	 * For portA and portB variable to effectively match the desired port,
	 * you have to supply a portA register address to reg. Otherwise, values
	 * will be reversed due to the way the MCP23017 works in Byte mode.
	 */
	void writeRegister(MCP23017Register reg, uint8_t portA, uint8_t portB);

};