// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GPIO_EXPANDER_HPP
#define XPCC_GPIO_EXPANDER_HPP

#include <xpcc/architecture/interface/gpio.hpp>
#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/processing/resumable.hpp>

namespace xpcc
{

/**
 * Interface of an IO expander driver.
 *
 * All IO expander driver should implement this interface, so that they can be wrapped
 * into `xpcc::GpioExpanderPin` and `xpcc::GpioExpanderPort`.
 *
 * The interface is almost identical to an GPIO pin, except multiple pins can be set at once.
 *
 * @note The driver must buffer the expanders IO registers. This means at least the direction,
 *       output and input values should be buffered so that a read-modify-write can be performed
 *       on SRAM and then only one write is performed on the bus instead of a read and write!
 *
 * @warning The `read()` function does not perform a read bus access, but operates on the
 *          buffered result of `readInput()`. This allows a capture-and-evaluate approach, which
 *          is especially useful for reducing bus traffic and necessary for sampling all pins at
 *          the same time for `xpcc::GpioExpanderPort`.
 *
 * @see xpcc::GpioExpanderPin
 * @see xpcc::GpioExpanderPort
 * @see xpcc::GpioIO
 *
 * @author  Niklas Hauser
 * @ingroup gpio
 */
class GpioExpander
{
#ifdef __DOXYGEN__
public:
	/// This holds the width of the port as number of bits
	/// and can be used in drivers to assert the correct requirements
	static constexpr uint8_t width;

public:
	/// A type containing a unique identifier for each pin.
	/// This is mostly a bit mask, however, since this is implementation
	/// defined it can also be an index.
	/// The underlying type depends on the width of the expander!
	enum class
	Pin : uint16_t
	{
		P0,
		P1,
		P2,
		P3,
		P4,
		P5,
		P6,
		P7
	};
	/// Use type-safe flags container for the pins
	typedef xpcc::Flags8<Pin> Pins;

public:
	/// Sets one or more pins to output
	xpcc::ResumableResult<bool>
	setOutput(Pins pins);

	/// Sets one or more pins to logic high
	/// @warning only modifies pins that have previously been set to output!
	xpcc::ResumableResult<bool>
	set(Pins pins);

	/// Resets one or more pins to logic low
	/// @warning only modifies pins that have previously been set to output!
	xpcc::ResumableResult<bool>
	reset(Pins pins);

	/// Toggles one or more pins
	/// @warning only modifies pins that have previously been set to output!
	xpcc::ResumableResult<bool>
	toggle(Pins pins);

	/// sets one or more pins to high or low level
	/// @warning only modifies pins that have previously been set to output!
	xpcc::ResumableResult<bool>
	set(Pins pins, bool value);

	/// Returns the set logical output state of the pin.
	bool
	isSet(Pin pin) const;

	/// returns direction of one pin at run-time
	xpcc::Gpio::Direction
	getDirection(Pin pin) const;

public:
	/// Sets one or more pins to input
	xpcc::ResumableResult<bool>
	setInput(Pins pins);

	/// Returns true if **all** pins have a high level
	/// @warning This function operates on the buffered input read.
	///          You have to call `readInput()` to update the result.
	bool
	read(Pins pins) const;

	/// Reads the inputs and buffers them
	xpcc::ResumableResult<bool>
	readInput();

public:
	/// Writes data to the entire port
	/// @warning only modifies pins that have previously been set to output!
	xpcc::ResumableResult<bool>
	writePort(uint16_t data);

	/// Reads the entire port, buffers them and outputs the result to data.
	xpcc::ResumableResult<bool>
	readPort(uint16_t &data);

public:
	/// Returns the direction bits: 0 for Input, 1 for Output
	Pins
	getDirections() const;

	/// Returns the output bits: 0 for low, 1 for high
	Pins
	getOutputs() const;

	/// Returns the input bits: 0 for low, 1 for high
	Pins
	getInputs() const;
#endif
};

/**
 * Create an xpcc GPIO compatible interface from any IO-expander
 * conforming to the xpcc::GpioExpander interface.
 *
 * This allows the transparent usage of IO Expander pins in classes that require
 * Gpios as template arguments.
 * @note For every pin access a blocking bus transfer is performed, therefore
 *       do not expect these gpios to be fast!
 *
 * Usage:
 * @code
 * typedef xpcc::Pca8574<MyI2cMaster> Expander;
 * Expander expander;
 *
 * typedef xpcc::GpioExpanderPin< Expander, expander, Expander::Pin::P0 > Rs;
 * @endcode
 *
 * However, instead of using this class like that, prefer using the alias-template
 * types provided by the IO-expander of your choice for better readability:
 *
 * @code
 * typedef xpcc::Pca8574<MyI2cMaster> Expander;
 * Expander expander;
 *
 * // use predefined pin types of the local IO-expander
 * typedef Expander::P0< expander > Rs;
 * typedef Expander::P1< expander > Rw;
 * typedef Expander::P2< expander > E;
 * typedef Expander::P3< expander > Backlight;
 *
 * Backlight::set();
 * @endcode
 *
 * @warning Access to the IO-expander is **blocking and can silently fail**!
 *          If this is undesirable for you application, you need to write your own wrapper!
 *
 * @see xpcc::GpioExpander
 * @see xpcc::GpioIO
 *
 * @tparam  GpioExpander    Type of class conforming to xpcc::GpioExpander interface
 * @tparam  expander        instance of the expander with the selected pin
 * @tparam  pin             pin identifier on expander
 *
 * @ingroup gpio
 * @author: strongly-typed
 * @author: Niklas Hauser
 */
template <
	typename GpioExpander,
	GpioExpander &expander,
	typename GpioExpander::Pin pin
	>
class GpioExpanderPin : public xpcc::GpioIO
{
public:
	static constexpr Direction direction = Direction::InOut;

public:
	static void
	setOutput()
	{
		RF_CALL_BLOCKING(expander.setOutput(pin));
	}

	static void inline
	setOutput(bool value)
	{
		set(value);
	}

	static void
	set()
	{
		RF_CALL_BLOCKING(expander.set(pin));
	}

	static void
	set(bool value)
	{
		RF_CALL_BLOCKING(expander.set(pin, value));
	}

	static void
	reset()
	{
		RF_CALL_BLOCKING(expander.reset(pin));
	}

	static void
	toggle()
	{
		RF_CALL_BLOCKING(expander.toggle(pin));
	}

	static bool inline
	isSet()
	{
		return expander.isSet(pin);
	}

	static void
	setInput()
	{
		RF_CALL_BLOCKING(expander.setInput(pin));
	}

	static bool
	read()
	{
		RF_CALL_BLOCKING(expander.readInput());

		return expander.read(pin);
	}

	static Direction
	getDirection()
	{
		return expander.getDirection(pin);
	}
};

} // xpcc namespace

#endif // XPCC_IO_EXPANDER_PIN_HPP
