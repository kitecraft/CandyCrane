#include "MCP23017.h"

void MCP23017::init(uint8_t address, TwoWire& bus)
{
	_deviceAddr = address;
	_bus = &bus;
	//BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : open drain output
	//INTPOL = 	0 : interrupt active low
	writeRegister(MCP23017Register::IOCON, 0b00100000);

	//enable all pull up resistors (will be effective for input pins only)
	writeRegister(MCP23017Register::GPPU_A, 0xFF, 0xFF);
}

void MCP23017::portMode(MCP23017Port port, uint8_t directions, uint8_t pullups, uint8_t inverted)
{
	writeRegister(MCP23017Register::IODIR_A + port, directions);
	writeRegister(MCP23017Register::GPPU_A + port, pullups);
	writeRegister(MCP23017Register::IPOL_A + port, inverted);
}

void MCP23017::writePort(MCP23017Port port, uint8_t value)
{
	writeRegister(MCP23017Register::GPIO_A + port, value);
}


void MCP23017::writeRegister(MCP23017Register reg, uint8_t value)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->write(value);
	_bus->endTransmission();
}

void MCP23017::writeRegister(MCP23017Register reg, uint8_t portA, uint8_t portB)
{
	_bus->beginTransmission(_deviceAddr);
	_bus->write(static_cast<uint8_t>(reg));
	_bus->write(portA);
	_bus->write(portB);
	_bus->endTransmission();
}
