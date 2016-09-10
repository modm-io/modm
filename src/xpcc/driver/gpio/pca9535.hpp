// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA9535_HPP
#define XPCC_PCA9535_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/architecture/interface/gpio_expander.hpp>
#include <xpcc/math/geometry/angle.hpp>

namespace xpcc
{

struct pca9535
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Command : uint8_t
	{
		InputPort0 = 0,
		InputPort1 = 1,
		OutputPort0 = 2,
		OutputPort1 = 3,
		PolarityInversionPort0 = 4,
		PolarityInversionPort1 = 5,
		ConfigurationPort0 = 6,
		ConfigurationPort1 = 7
	};
	/// @endcond

public:
	enum class
	Pin : uint16_t
	{
		P0_0 = (1 << 0),
		P0_1 = (1 << 1),
		P0_2 = (1 << 2),
		P0_3 = (1 << 3),
		P0_4 = (1 << 4),
		P0_5 = (1 << 5),
		P0_6 = (1 << 6),
		P0_7 = (1 << 7),

		P1_0 = (1 << 8),
		P1_1 = (1 << 9),
		P1_2 = (1 << 10),
		P1_3 = (1 << 11),
		P1_4 = (1 << 12),
		P1_5 = (1 << 13),
		P1_6 = (1 << 14),
		P1_7 = (1 << 15)
	};
	typedef xpcc::Flags16<Pin> Pins;
	XPCC_INT_TYPE_FLAGS(Pins);
}; // struct pca9535

/**
 * PCA9535 I2C GPIO expander.
 *
 * Also software-compatible with PCA9555.
 * The lower three address bits can be configured: 0100abc.
 *
 * Notice that you can specify multiple pins at the same time for functions
 * with argument type `Pins`, either by ORing the according pins, or
 * converting a 16bit value using the `Pins(uint16_t)` converting constructor.
 *
 * Other functions with argument type `Pin` can only take one pin.
 * If you want to operate on all 16bit, use the `get(Inputs|Outputs|Directions|Polarities)()`
 * getters.
 *
 * @code
 * typedef xpcc::pca9535::Pin Pin;
 * typedef xpcc::pca9535::Pins Pins;
 *
 * RF_CALL_BLOCKING(expander.setOutput(Pins(0xff));	// set all lower 8 pins to output
 * RF_CALL_BLOCKING(expander.set(Pin::P0_0));		// set only pin 00 high
 * RF_CALL_BLOCKING(expander.reset(Pin::P0_1 | Pin::P0_2));	// reset 2 pins
 * RF_CALL_BLOCKING(expander.toggle(Pin::P0_3 | Pin::P0_4 | Pin::P0_5));	// toggle 3 pins
 *
 * RF_CALL_BLOCKING(expander.readInput());	// transfer input states from the external chip
 * bool high = expander.read(Pin::P1_0);	// check a single pin
 *
 * Pins input = expander.getInputs();	// get all 16 input states
 * bool isAnyPinHigh = input.any(Pin::P1_1 | Pin::P1_2 | Pin::P1_3); // check if any of 3 pins is high
 * @endcode
 *
 * @author	Niklas Hauser
 * @ingroup driver_gpio
 */
template < typename I2cMaster >
class Pca9535 : public pca9535, public xpcc::I2cDevice< I2cMaster, 2 >, public xpcc::GpioExpander
{
	enum class
	Index : uint8_t
	{
		Input = 0,
		Output = 3,
		Polarity = 6,
		Configuration = 9
	};

public:
	static constexpr uint8_t width = 16;

	using PortType = uint16_t;

	static constexpr uint8_t
	indexFromPin(Pin pin)
	{
		return xpcc::leftmostBit(PortType(pin));
	}

public:
	/// Constructor, sets address to default of 0x20 (range 0x20 - 0x27)
	Pca9535(uint8_t address=0x20);

public:
	xpcc::ResumableResult<bool>
	setOutput(Pins pins);

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
		return memory.output.any(pin);
	}

	xpcc::Gpio::Direction inline
	getDirection(Pin pin) const
	{
		// output is 0, input is 1
		return memory.configuration.any(pin) ?
				xpcc::Gpio::Direction::In :
				xpcc::Gpio::Direction::Out;
	}

public:
	xpcc::ResumableResult<bool>
	setInput(Pins pins);

	xpcc::ResumableResult<bool>
	setInvertInput(Pins pins);

	xpcc::ResumableResult<bool>
	resetInvertInput(Pins pins);

	bool inline
	read(Pins pins) const
	{
		// high is 1, low is 0
		return memory.input.all(pins);
	}

	xpcc::ResumableResult<bool> inline
	readInput()
	{ return readMemory(Index::Input); }

public:
	xpcc::ResumableResult<bool>
	writePort(PortType data);

	xpcc::ResumableResult<bool>
	readPort(PortType &data);


public:
	Pins inline
	getDirections() const
	{ return ~memory.configuration; }

	Pins inline
	getOutputs() const
	{ return memory.output; }

	Pins inline
	getInputs() const
	{ return memory.input; }

	Pins inline
	getPolarities() const
	{ return memory.polarity; }

public:
	/// Alias-templates for simpler use of the Pin
	/// @{
	template < Pca9535<I2cMaster> &object >
	using P0_0 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_0 >;
	template < Pca9535<I2cMaster> &object >
	using P0_1 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_1 >;
	template < Pca9535<I2cMaster> &object >
	using P0_2 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_2 >;
	template < Pca9535<I2cMaster> &object >
	using P0_3 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_3 >;
	template < Pca9535<I2cMaster> &object >
	using P0_4 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_4 >;
	template < Pca9535<I2cMaster> &object >
	using P0_5 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_5 >;
	template < Pca9535<I2cMaster> &object >
	using P0_6 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_6 >;
	template < Pca9535<I2cMaster> &object >
	using P0_7 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P0_7 >;

	template < Pca9535<I2cMaster> &object >
	using P1_0 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_0 >;
	template < Pca9535<I2cMaster> &object >
	using P1_1 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_1 >;
	template < Pca9535<I2cMaster> &object >
	using P1_2 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_2 >;
	template < Pca9535<I2cMaster> &object >
	using P1_3 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_3 >;
	template < Pca9535<I2cMaster> &object >
	using P1_4 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_4 >;
	template < Pca9535<I2cMaster> &object >
	using P1_5 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_5 >;
	template < Pca9535<I2cMaster> &object >
	using P1_6 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_6 >;
	template < Pca9535<I2cMaster> &object >
	using P1_7 = GpioExpanderPin< Pca9535<I2cMaster>, object, Pin::P1_7 >;
	/// @}

	/// Alias-templates for simpler use of the Port
	template < Pca9535<I2cMaster> &object, Pin StartPin, uint8_t Width, GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal  >
	using Port = GpioExpanderPort< Pca9535<I2cMaster>, object, StartPin, Width, DataOrder >;

private:
	xpcc::ResumableResult<bool>
	writeMemory(Index index);

	xpcc::ResumableResult<bool>
	readMemory(Index index);

	/// @cond
	struct xpcc_packed
	Memory
	{
		Memory() :
			commandInput(Command::InputPort0),
			input(0),
			commandOutput(Command::OutputPort0),
			output(0),
			commandPolarity(Command::PolarityInversionPort0),
			polarity(0),
			commandConfiguration(Command::ConfigurationPort0),
			configuration(0)
		{}

		const Command commandInput;
		Pins input;

		const Command commandOutput;
		Pins output;

		const Command commandPolarity;
		Pins polarity;

		const Command commandConfiguration;
		Pins configuration;
	};

	union
	{
		Memory memory;
		uint8_t buffer[4 * 3];
	};
	/// @endcond
};

}	// namespace xpcc

#include "pca9535_impl.hpp"

#endif // XPCC_PCA9535_HPP
