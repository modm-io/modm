/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP23X08_HPP
#define MODM_MCP23X08_HPP

#include <modm/architecture/interface/gpio_expander.hpp>
#include <modm/architecture/interface/register.hpp>
#include "mcp23_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_mcp23x08
struct mcp23x08
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		IODIR = 0x00,		///< Port direction (1=input, 0=output)
		IPOL = 0x01,		///< Invert polarity
		GPINTEN = 0x02,		///< Enable interrupt
		DEFVAL = 0x03,		///< Compare register for interrupt
		INTCON = 0x04,
		IOCON = 0x05,		///< Configuration
		GPPU = 0x06,		///< Enable pullups
		INTF = 0x07,		///< Interrupt flag register
		INTCAP = 0x08,		///< Interrupt capture register
		GPIO = 0x09,		///< Port values
		OLAT = 0x0A			///< Output latch register
	};

	enum class
	IoCon : uint8_t
	{
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
	Pin : uint8_t
	{
		P0 = Bit0,
		P1 = Bit1,
		P2 = Bit2,
		P3 = Bit3,
		P4 = Bit4,
		P5 = Bit5,
		P6 = Bit6,
		P7 = Bit7,
	};
	typedef modm::Flags8<Pin> Pins;
	MODM_INT_TYPE_FLAGS(Pins);
}; // struct mcp23x08

/**
 * @see Mcp23TransportI2c
 * @see Mcp23TransportSpi
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 *
 * @author	Niklas Hauser
 *
 * @ingroup modm_driver_mcp23x08
 */
template <class Transport>
class Mcp23x08 : public mcp23x08, public Transport, public modm::GpioExpander
{
public:
	static constexpr uint8_t width = 8;

	using PortType = uint8_t;

	static constexpr uint8_t
	indexFromPin(Pin pin)
	{
		return modm::leftmostBit(PortType(pin));
	}

public:
	/// Constructor, sets address to default of 0x20 (range 0x20 - 0x27)
	Mcp23x08(uint8_t address=0x20);

public:
	bool
	initialize();

	bool
	setOutput(Pins pins);

	bool
	set(Pins pins);

	bool
	reset(Pins pins);

	bool
	toggle(Pins pins);

	bool
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
	bool
	setInput(Pins pins);

	bool
	setPullUp(Pins pins);

	bool
	resetPullUp(Pins pins);

	bool
	setInvertInput(Pins pins);

	bool
	resetInvertInput(Pins pins);

	bool
	read(Pin pin)
	{
		// high is 1, low is 0
		return memory.gpio.any(pin);
	}

	bool inline
	readInput()
	{ return Transport::read(i(Register::GPIO), buffer + 9, 2); }

	bool inline
	readAllInput()
	{ return Transport::read(i(Register::INTF), buffer + 7, 8); }

public:
	bool
	writePort(PortType data);

	bool
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
	template < Mcp23x08<Transport> &object >
	using P0 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P0 >;
	template < Mcp23x08<Transport> &object >
	using P1 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P1 >;
	template < Mcp23x08<Transport> &object >
	using P2 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P2 >;
	template < Mcp23x08<Transport> &object >
	using P3 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P3 >;
	template < Mcp23x08<Transport> &object >
	using P4 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P4 >;
	template < Mcp23x08<Transport> &object >
	using P5 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P5 >;
	template < Mcp23x08<Transport> &object >
	using P6 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P6 >;
	template < Mcp23x08<Transport> &object >
	using P7 = GpioExpanderPin< Mcp23x08<Transport>, object, Pin::P7 >;
	/// @}

	/// Alias-templates for simpler use of the Port
	template < Mcp23x08<Transport> &object, Pin StartPin, uint8_t Width, GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal  >
	using Port = GpioExpanderPort< Mcp23x08<Transport>, object, StartPin, Width, DataOrder >;

private:
	struct modm_packed
	Memory
	{
		Memory() :
			direction(0xff),
			polarity(0),
			interruptEnable(0),
			interruptDefault(0),
			interruptControl(0),
			control(0),
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
		IoCon_t control;		// IOCON
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

#include "mcp23x08_impl.hpp"

#endif	// MODM_MCP23X08_HPP
