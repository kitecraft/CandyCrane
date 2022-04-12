#include "ButtonManager.h"

ButtonManager::ButtonManager()
{

}

ButtonManager::~ButtonManager()
{
	if (mcpController != NULL)
	{
		delete mcpController;
		mcpController = NULL;
	}
}

void ButtonManager::Init(uint8_t mcp23017Address, int interruptPin)
{
	mcpController = new MCP23017(mcp23017Address);
	_mcInterruptPin = interruptPin;

	mcpController->init();
	mcpController->portMode(MCP23017Port::A, 0b11111111, 0b11111111);
	mcpController->portMode(MCP23017Port::B, 0b11111111, 0b11111111);

	mcpController->interruptMode(MCP23017InterruptMode::Or);
	mcpController->interrupt(MCP23017Port::A, CHANGE);
	mcpController->interrupt(MCP23017Port::B, CHANGE);

	mcpController->writeRegister(MCP23017Register::IPOL_A, 0x00);
	mcpController->writeRegister(MCP23017Register::IPOL_B, 0x00);


	pinMode(_mcInterruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(_mcInterruptPin), McpButtonInterruptFunction, FALLING);
	mcpController->clearInterrupts();
}

int ButtonManager::HandleButtonInterrupt()
{
	if (g_mcpButtonController_Interrupted)
	{
		g_mcpButtonController_Interrupted = false;
		return GetLastButtonPressed();
	}
	return NO_PIN;
}

int ButtonManager::GetLastButtonPressed()
{
	int pin = getLastInterruptPin();
	bool action = getLastInterruptPinValue();
	mcpController->clearInterrupts();
	if (action == BUTTON_PRESSED) {
		if (_buttons[pin].Ready())
		{
			return pin;
		}
	}
	return NO_PIN;
}



int8_t ButtonManager::getLastInterruptPin()
{
	uint8_t intf;

	// try port A
	intf = mcpController->readRegister(MCP23017Register::INTF_A);
	for (int i = 0; i < 8; i++)
		if (bitRead(intf, i))
			return i;

	// try port B
	intf = mcpController->readRegister(MCP23017Register::INTF_B);
	for (int i = 0; i < 8; i++)
		if (bitRead(intf, i))
			return i + 8;

	return MCP23017_INT_ERR;
}

uint8_t ButtonManager::getLastInterruptPinValue()
{
	uint8_t intPin = getLastInterruptPin();
	if (intPin != MCP23017_INT_ERR) {
		uint8_t intcapreg = regForPin(intPin, MCP23017_INTCAPA, MCP23017_INTCAPB);
		uint8_t bit = bitForPin(intPin);
		return (mcpController->readRegister(intcapreg) >> bit) & (0x01);
	}

	return MCP23017_INT_ERR;
}
uint8_t ButtonManager::bitForPin(uint8_t pin)
{
	return pin % 8;
}

uint8_t ButtonManager::regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr)
{
	return (pin < 8) ? portAaddr : portBaddr;
}