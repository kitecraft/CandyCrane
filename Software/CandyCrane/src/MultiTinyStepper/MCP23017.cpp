#include "MCP23017.h"

void MCP23017::init(uint8_t address, TwoWire& bus)
{
	_deviceAddr = address;
	_bus = &bus;
	writeRegister(MCP23017Register::IOCON, 0b00100000);
	portMode(MCP23017Port::A, 0);
	portMode(MCP23017Port::B, 0);

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
