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
#include <xpcc/math/geometry/angle.hpp>

namespace xpcc
{

struct pca9535 : public Gpio
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
class Pca9535 : public pca9535, public xpcc::I2cDevice< I2cMaster, 2 >
{
	enum class
	Index : uint8_t
	{
		Input = 0,
		Output = 3,
		Configuration = 6,
		Polarity = 9
	};

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

	bool
	isSet(Pin pin)
	{
		// high is 1, low is 0
		return memory.output.any(pin);
	}

	Direction
	getDirection(Pin pin)
	{
		// output is 0, input is 1
		return memory.configuration.any(pin) ? Direction::In : Direction::Out;
	}

public:
	xpcc::ResumableResult<bool>
	setInput(Pins pins);

	xpcc::ResumableResult<bool>
	setInvertInput(Pins pins);

	xpcc::ResumableResult<bool>
	resetInvertInput(Pins pins);

	bool
	read(Pin pin)
	{
		// high is 1, low is 0
		return memory.input.any(pin);
	}

public:
	xpcc::ResumableResult<bool> inline
	readInput()
	{ return readMemory(Index::Input); }

public:
	Pins inline
	getOutputs()
	{ return memory.output; }

	Pins inline
	getInputs()
	{ return memory.input; }

	/// 0 is input, 1 is output
	Pins inline
	getDirections()
	{ return memory.configuration ^ Pins(0xffff); }

	Pins inline
	getPolarities()
	{ return memory.polarity; }

private:
	xpcc::ResumableResult<bool>
	writeMemory(Index index);

	xpcc::ResumableResult<bool>
	readMemory(Index index);

	/// @cond
	struct ATTRIBUTE_PACKED
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

		const uint8_t commandInput;
		Pins input;

		const uint8_t commandOutput;
		Pins output;

		const uint8_t commandPolarity;
		Pins polarity;

		const uint8_t commandConfiguration;
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
