/*
 * Copyright (c) 2015-2016, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP23X17_HPP
#define MODM_MCP23X17_HPP

#include <modm/architecture/interface/gpio_expander.hpp>
#include <modm/architecture/interface/register.hpp>
#include "mcp23_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_mcp23x17
struct mcp23x17
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		IODIR = 0x00,		///< Port direction (1=input, 0=output)
		IPOL = 0x02,		///< Invert polarity
		GPINTEN = 0x04,		///< Enable interrupt
		DEFVAL = 0x06,		///< Compare register for interrupt
		INTCON = 0x08,
		IOCON = 0x0A,		///< Configuration
		GPPU = 0x0C,		///< Enable pullups
		INTF = 0x0E,		///< Interrupt flag register
		INTCAP = 0x10,		///< Interrupt capture register
		GPIO = 0x12,		///< Port values
		OLAT = 0x14			///< Output latch register
	};

	enum class
	IoCon : uint8_t
	{
		Bank = Bit7,	///< Controls how the registers are addressed
		Mirror = Bit6,	///< INT Pins Mirror bit
		SeqOp = Bit5,	///< Sequential Operation mode bit
		DisSlw = Bit4,	///< Slew Rate control bit for SDA output
		HaEn = Bit3,	///< Hardware Address Enable bit
		Odr = Bit2,		///< This bit configures the INT pin as an open-drain output
		IntPol = Bit1	///< This bit sets the polarity of the INT output pin
	};
	MODM_FLAGS8(IoCon);

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond

public:
	enum class
	Pin : uint16_t
	{
		A0 = Bit0,
		A1 = Bit1,
		A2 = Bit2,
		A3 = Bit3,
		A4 = Bit4,
		A5 = Bit5,
		A6 = Bit6,
		A7 = Bit7,

		B0 = Bit8,
		B1 = Bit9,
		B2 = Bit10,
		B3 = Bit11,
		B4 = Bit12,
		B5 = Bit13,
		B6 = Bit14,
		B7 = Bit15
	};
	typedef modm::Flags16<Pin> Pins;
	MODM_INT_TYPE_FLAGS(Pins);
}; // struct mcp23x17

/**
 * @see Mcp23TransportI2c
 * @see Mcp23TransportSpi
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 *
 * @ingroup modm_driver_mcp23x17
 */
template <class Transport>
class Mcp23x17 : public mcp23x17, public Transport, public modm::GpioExpander
{
public:
	static constexpr uint8_t width = 16;

	using PortType = uint16_t;

	static constexpr uint8_t
	indexFromPin(Pin pin)
	{
		return modm::leftmostBit(PortType(pin));
	}

public:
	/// Constructor, sets address to default of 0x20 (range 0x20 - 0x27)
	Mcp23x17(uint8_t address=0x20);

public:
	modm::ResumableResult<bool>
	initialize();

	modm::ResumableResult<bool>
	setOutput(Pins pins);

	modm::ResumableResult<bool>
	set(Pins pins);

	modm::ResumableResult<bool>
	reset(Pins pins);

	modm::ResumableResult<bool>
	toggle(Pins pins);

	modm::ResumableResult<bool>
	set(Pins pins, bool value);

	bool
	isSet(Pin pin)
	{
		// high is 1, low is 0
		return memory.outputLatch.any(pin);
	}

	modm::Gpio::Direction
	getDirection(Pin pin)
	{
		// output is 0, input is 1
		return memory.direction.any(pin) ?
				modm::Gpio::Direction::In :
				modm::Gpio::Direction::Out;
	}

public:
	modm::ResumableResult<bool>
	setInput(Pins pins);

	modm::ResumableResult<bool>
	setPullUp(Pins pins);

	modm::ResumableResult<bool>
	resetPullUp(Pins pins);

	modm::ResumableResult<bool>
	setInvertInput(Pins pins);

	modm::ResumableResult<bool>
	resetInvertInput(Pins pins);

	bool
	read(Pin pin)
	{
		// high is 1, low is 0
		return memory.gpio.any(pin);
	}

	modm::ResumableResult<bool> inline
	readInput()
	{ return Transport::read(i(Register::GPIO), buffer + 18, 2); }

	modm::ResumableResult<bool> inline
	readAllInput()
	{ return Transport::read(i(Register::INTF), buffer + 14, 8); }

public:
	modm::ResumableResult<bool>
	writePort(PortType data);

	modm::ResumableResult<bool>
	readPort(PortType &data);

public:
	Pins inline
	getDirections()
	{ return ~memory.direction; }

	Pins inline
	getOutputs()
	{ return memory.outputLatch; }

	Pins inline
	getInputs()
	{ return memory.gpio; }

	Pins inline
	getPolarities()
	{ return memory.polarity; }

public:
	/// Alias-templates for simpler use of the Pin
	/// @{
	template < Mcp23x17<Transport> &object >
	using A0 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A0 >;
	template < Mcp23x17<Transport> &object >
	using A1 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A1 >;
	template < Mcp23x17<Transport> &object >
	using A2 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A2 >;
	template < Mcp23x17<Transport> &object >
	using A3 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A3 >;
	template < Mcp23x17<Transport> &object >
	using A4 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A4 >;
	template < Mcp23x17<Transport> &object >
	using A5 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A5 >;
	template < Mcp23x17<Transport> &object >
	using A6 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A6 >;
	template < Mcp23x17<Transport> &object >
	using A7 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::A7 >;

	template < Mcp23x17<Transport> &object >
	using B0 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B0 >;
	template < Mcp23x17<Transport> &object >
	using B1 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B1 >;
	template < Mcp23x17<Transport> &object >
	using B2 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B2 >;
	template < Mcp23x17<Transport> &object >
	using B3 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B3 >;
	template < Mcp23x17<Transport> &object >
	using B4 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B4 >;
	template < Mcp23x17<Transport> &object >
	using B5 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B5 >;
	template < Mcp23x17<Transport> &object >
	using B6 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B6 >;
	template < Mcp23x17<Transport> &object >
	using B7 = GpioExpanderPin< Mcp23x17<Transport>, object, Pin::B7 >;
	/// @}

	/// Alias-templates for simpler use of the Port
	template < Mcp23x17<Transport> &object, Pin StartPin, uint8_t Width, GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal  >
	using Port = GpioExpanderPort< Mcp23x17<Transport>, object, StartPin, Width, DataOrder >;

private:
	struct modm_packed
	Memory
	{
		Memory() :
			direction(0xffff),
			polarity(0),
			interruptEnable(0),
			interruptDefault(0),
			interruptControl(0),
			controlA(0), controlB(0),
			pullup(0),
			interruptFlag(0),
			interruptCapture(0),
			gpio(0),
			outputLatch(0)
		{}

		Pins direction;			// IODIR
		Pins polarity;			// IPOL
		Pins interruptEnable;	// GPINTEN
		Pins interruptDefault;	// DEFVAL
		Pins interruptControl;	// INTCON
		IoCon_t controlA;		// IOCONA
		IoCon_t controlB;		// IOCONB
		Pins pullup;			// GPPU
		Pins interruptFlag;		// INTF
		Pins interruptCapture;	// INTCAP
		Pins gpio;				// GPIO
		Pins outputLatch;		// OLAT
	};

	union
	{
		Memory memory;
		uint8_t buffer[sizeof(Memory)];
	};
};

}	// namespace modm

#include "mcp23x17_impl.hpp"

#endif	// MODM_MCP23X17_HPP
