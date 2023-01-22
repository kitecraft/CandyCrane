#pragma once
#include <Arduino.h>

#define MCP23017_DEFAULT_ADDR 0x20
#define MTS_STEPPER_TYPE_16_REV 513
#define MTS_STEPPER_TYPE_64_REV 2038

enum MTS_STEPPER_TYPE {
	MTS_STEPPER_TYPE_16 = 0,
	MTS_STEPPER_TYPE_64
};

enum MTS_STEPPER_STEP_SIZE {
	MTS_STEPPER_FULL_STEP = 0,
	MTS_STEPPER_HALF_STEP
};

enum MTS_STEPPER_ID {
	MTS_STEPPER_1 = 0,
	MTS_STEPPER_2,
	MTS_STEPPER_3,
	MTS_STEPPER_4
};

//Everything below is used internally
#define MTS_STEPPER_COUNT 4
enum MTS_STEPPER_PHASE {
	MTS_STEPPER_PHASE_1 = 0,
	MTS_STEPPER_PHASE_2,
	MTS_STEPPER_PHASE_3,
	MTS_STEPPER_PHASE_4,
	MTS_STEPPER_PHASE_5,
	MTS_STEPPER_PHASE_6,
	MTS_STEPPER_PHASE_7,
	MTS_STEPPER_PHASE_8,

	MTS_STEPPER_PHASE_DISABLE = -1
};

enum STEPPER_MOVE_ERROR {
	SME_NONE = 0,
	SME_LIMIT_SWITCH_ACTIVE,
	SME_MINIMUM_TRAVEL_LIMIT_REACHED,
	SME_MAXIMUM_TRAVEL_LIMIT_REACHED
};
#define _MCP23017_INTERRUPT_SUPPORT_ ///< Enables support for MCP23017 interrupts.
#define MCP23017_INT_ERR 255 //!< Interrupt error

enum class MCP23017Port : uint8_t
{
	A = 0,
	B = 1
};
/**
 * Registers addresses.
 * The library use addresses for IOCON.BANK = 0.
 * See "3.2.1 Byte mode and Sequential mode".
 */
enum class MCP23017Register : uint8_t
{
	IODIR_A = 0x00, 		///< Controls the direction of the data I/O for port A.
	IODIR_B = 0x01,			///< Controls the direction of the data I/O for port B.
	IPOL_A = 0x02,			///< Configures the polarity on the corresponding GPIO_ port bits for port A.
	IPOL_B = 0x03,			///< Configures the polarity on the corresponding GPIO_ port bits for port B.
	GPINTEN_A = 0x04,			///< Controls the interrupt-on-change for each pin of port A.
	GPINTEN_B = 0x05,			///< Controls the interrupt-on-change for each pin of port B.
	DEFVAL_A = 0x06,			///< Controls the default comparaison value for interrupt-on-change for port A.
	DEFVAL_B = 0x07,			///< Controls the default comparaison value for interrupt-on-change for port B.
	INTCON_A = 0x08,			///< Controls how the associated pin value is compared for the interrupt-on-change for port A.
	INTCON_B = 0x09,			///< Controls how the associated pin value is compared for the interrupt-on-change for port B.
	IOCON = 0x0A,			///< Controls the device.
	GPPU_A = 0x0C,			///< Controls the pull-up resistors for the port A pins.
	GPPU_B = 0x0D,			///< Controls the pull-up resistors for the port B pins.
	INTF_A = 0x0E,			///< Reflects the interrupt condition on the port A pins.
	INTF_B = 0x0F,			///< Reflects the interrupt condition on the port B pins.
	INTCAP_A = 0x10,			///< Captures the port A value at the time the interrupt occured.
	INTCAP_B = 0x11,			///< Captures the port B value at the time the interrupt occured.
	GPIO_A = 0x12,			///< Reflects the value on the port A.
	GPIO_B = 0x13,			///< Reflects the value on the port B.
	OLAT_A = 0x14,			///< Provides access to the port A output latches.
	OLAT_B = 0x15,			///< Provides access to the port B output latches.
};

inline MCP23017Register operator+(MCP23017Register a, MCP23017Port b) {
	return static_cast<MCP23017Register>(static_cast<uint8_t>(a) + static_cast<uint8_t>(b));
};