#pragma once

#include <Arduino.h>
#include <MCP23017.h>
#include "IceButton.h"

#define NUMBER_BUTTONS 16
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

#define MCP23017_INT_ERR 255 //!< Interrupt error
#define MCP23017_INTCAPA 0x10  //!< Interrupt captured value for port register A
#define MCP23017_INTCAPB 0x11  //!< Interrupt captured value for port register B

#define NO_PIN -1

static volatile bool g_mcpButtonController_Interrupted = false;
static void IRAM_ATTR McpButtonInterruptFunction()
{
	g_mcpButtonController_Interrupted = true;
}

class ButtonManager
{
private:
	IceButton _buttons[NUMBER_BUTTONS];
	int _mcInterruptPin = -1;

	MCP23017 *mcpController = NULL;
	int8_t getLastInterruptPin();
	uint8_t getLastInterruptPinValue();
	uint8_t regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr);
	uint8_t bitForPin(uint8_t pin);

	int GetLastButtonPressed();

public:
	ButtonManager();
	~ButtonManager();
	void Init(uint8_t mcp23017Address, int interruptPin);
	// Returns the ID of the pin
	// -1 in no pin
	int HandleButtonInterrupt();
};