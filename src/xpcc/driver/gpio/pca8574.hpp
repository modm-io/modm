// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA8574_HPP
#define XPCC_PCA8574_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/architecture/interface/gpio_expander.hpp>

namespace xpcc
{

struct pca8574
{
public:
	enum class
	Pin : uint8_t
	{
		P0 = (1 << 0),
		P1 = (1 << 1),
		P2 = (1 << 2),
		P3 = (1 << 3),
		P4 = (1 << 4),
		P5 = (1 << 5),
		P6 = (1 << 6),
		P7 = (1 << 7)
	};
	typedef xpcc::Flags8<Pin> Pins;
	XPCC_INT_TYPE_FLAGS(Pins);
}; // struct pca8574

/**
 * PCA8574: Remote 8-bit I/O expander for I2C-bus with interrupt
 *
 * This IO-Expander only has open-drain outputs with weak internal pull-ups.
 *
 * @ingroup driver_gpio
 *
 * @author  strongly-typed
 * @author  Niklas Hauser
 */
template < class I2cMaster >
class Pca8574 : public pca8574, public xpcc::I2cDevice< I2cMaster, 2 >, public xpcc::GpioExpander
{
public:
	static constexpr uint8_t width = 8;

	using PortType = uint8_t;

	static constexpr uint8_t
	indexFromPin(Pin pin)
	{
		return xpcc::leftmostBit(PortType(pin));
	}

public:
	/// Constructor, sets address to default of 0x27
	Pca8574(uint8_t address=0x27);

public:
	xpcc::ResumableResult<bool> inline
	setOutput(Pins pins)
	{
		// nothing needs to be changed for switching to output
		direction.set(pins);
		return {xpcc::rf::Stopped, true};
	}

	xpcc::ResumableResult<bool>
	set(Pins pins);

	xpcc::ResumableResult<bool>
	reset(Pins pins);

	xpcc::ResumableResult<bool>
	toggle(Pins pins);

	xpcc::ResumableResult<bool>
	set(Pins pins, bool value);

	bool inline
	isSet(Pin pin) const
	{
		// high is 1, low is 0
		return output.any(pin);
	}

	xpcc::Gpio::Direction inline
	getDirection(Pin /*pin*/) const
	{
		// all pins are always in open-drain mode
		return xpcc::Gpio::Direction::InOut;
	}

public:
	xpcc::ResumableResult<bool>
	setInput(Pins pins);

	bool inline
	read(Pins pins) const
	{
		// high is 1, low is 0
		return input.all(pins);
	}

public:
	xpcc::ResumableResult<bool> inline
	readInput()
	{ return readPort(input.value); }

public:
	xpcc::ResumableResult<bool>
	writePort(PortType value);

	xpcc::ResumableResult<bool>
	readPort(PortType &value);

public:
	Pins inline
	getOutputs()
	{ return output; }

	Pins inline
	getInputs()
	{ return input; }

	/// 0 is input, 1 is output
	Pins inline
	getDirections()
	{ return direction; }

public:
	/// Alias-templates for simpler use of the Pin
	/// @{
	template < Pca8574<I2cMaster> &object >
	using P0 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P0 >;
	template < Pca8574<I2cMaster> &object >
	using P1 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P1 >;
	template < Pca8574<I2cMaster> &object >
	using P2 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P2 >;
	template < Pca8574<I2cMaster> &object >
	using P3 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P3 >;
	template < Pca8574<I2cMaster> &object >
	using P4 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P4 >;
	template < Pca8574<I2cMaster> &object >
	using P5 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P5 >;
	template < Pca8574<I2cMaster> &object >
	using P6 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P6 >;
	template < Pca8574<I2cMaster> &object >
	using P7 = GpioExpanderPin< Pca8574<I2cMaster>, object, Pin::P7 >;
	/// @}

	/// Alias-templates for simpler use of the Port
	template < Pca8574<I2cMaster> &object, Pin StartPin, uint8_t Width >
	using Port = GpioExpanderPort< Pca8574<I2cMaster>, object, StartPin, Width >;

private:
	// buffer the io states
	Pins direction; // output = 1, input = 0
	Pins output;    // high = 1, low = 0
	Pins input;     // high = 1, low = 0
};

} // namespace xpcc

#include "pca8574_impl.hpp"

#endif // XPCC_PCA8574_HPP
