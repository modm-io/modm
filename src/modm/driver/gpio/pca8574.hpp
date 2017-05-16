/*
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA8574_HPP
#define MODM_PCA8574_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/architecture/interface/gpio_expander.hpp>

namespace modm
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
	typedef modm::Flags8<Pin> Pins;
	MODM_INT_TYPE_FLAGS(Pins);
}; // struct pca8574

/**
 * PCA8574: Remote 8-bit I/O expander for I2C-bus with interrupt
 *
 * This IO-Expander only has open-drain outputs with weak internal pull-ups.
 *
 * @ingroup driver_gpio
 *
 * @author  Sascha Schade
 * @author  Niklas Hauser
 */
template < class I2cMaster >
class Pca8574 : public pca8574, public modm::I2cDevice< I2cMaster, 2 >, public modm::GpioExpander
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
	/// Constructor, sets address to default of 0x27
	Pca8574(uint8_t address=0x27);

public:
	modm::ResumableResult<bool> inline
	setOutput(Pins pins)
	{
		// nothing needs to be changed for switching to output
		direction.set(pins);
		return {modm::rf::Stopped, true};
	}

	modm::ResumableResult<bool>
	set(Pins pins);

	modm::ResumableResult<bool>
	reset(Pins pins);

	modm::ResumableResult<bool>
	toggle(Pins pins);

	modm::ResumableResult<bool>
	set(Pins pins, bool value);

	bool inline
	isSet(Pin pin) const
	{
		// high is 1, low is 0
		return output.any(pin);
	}

	modm::Gpio::Direction inline
	getDirection(Pin /*pin*/) const
	{
		// all pins are always in open-drain mode
		return modm::Gpio::Direction::InOut;
	}

public:
	modm::ResumableResult<bool>
	setInput(Pins pins);

	bool inline
	read(Pins pins) const
	{
		// high is 1, low is 0
		return input.all(pins);
	}

public:
	modm::ResumableResult<bool> inline
	readInput()
	{ return readPort(input.value); }

public:
	modm::ResumableResult<bool>
	writePort(PortType value);

	modm::ResumableResult<bool>
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
	template < Pca8574<I2cMaster> &object, Pin StartPin, uint8_t Width, GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal >
	using Port = GpioExpanderPort< Pca8574<I2cMaster>, object, StartPin, Width, DataOrder >;

private:
	// buffer the io states
	Pins direction; // output = 1, input = 0
	Pins output;    // high = 1, low = 0
	Pins input;     // high = 1, low = 0
};

} // namespace modm

#include "pca8574_impl.hpp"

#endif // MODM_PCA8574_HPP
