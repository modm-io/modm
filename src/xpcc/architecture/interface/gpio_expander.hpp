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
#include <xpcc/math/utils/bit_operation.hpp>

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

	/// This contains the underlying type that can hold the bits of the entire port.
	/// This value is implementation defined, it may also be `uint16_t`.
	using PortType = uint8_t;

public:
	/// A type containing a unique identifier for each pin.
	/// This is mostly a bit mask, however, since this is implementation
	/// defined it can also be an index.
	enum class
	Pin : PortType;
	/// Use type-safe flags container for the pins
	typedef xpcc::Flags<Pin, PortType> Pins;

	/// Returns the *index* of the pin as a constexpr function.
	/// The mapping is implementation defined, however, as an implementation hint
	/// when using a bit mask, you should use `xpcc::leftmostBit()`
	static constexpr uint8_t
	indexFromPin(Pin pin);

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
	writePort(PortType data);

	/// Reads the entire port, buffers them and outputs the result to data.
	xpcc::ResumableResult<bool>
	readPort(PortType &data);

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
 * @author  strongly-typed
 * @author  Niklas Hauser
 */
template <
	typename GpioExpander,
	GpioExpander &expander,
	typename GpioExpander::Pin pin >
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

template <
	typename GpioExpander,
	GpioExpander &expander,
	typename GpioExpander::Pin StartPin,
	uint8_t Width,
	GpioPort::DataOrder DataOrder = GpioPort::DataOrder::Normal >
class GpioExpanderPort : public xpcc::GpioPort
{
	static constexpr uint8_t StartIndex = GpioExpander::indexFromPin(StartPin);

	static_assert(Width <= GpioExpander::width, "Width too large.");
	static_assert(Width > 0, "Width should be at least 1.");
	static_assert(StartIndex + Width <= GpioExpander::width, "StartPin + Width too large.");

	using Pins = typename GpioExpander::Pins;
	using PortType = typename GpioExpander::PortType;

	static constexpr PortType dataMask = (1 << Width) - 1;
	static constexpr PortType portMask = dataMask << StartIndex;

public:
	static constexpr uint8_t width = Width;

public:
	static void
	setOutput()
	{
		RF_CALL_BLOCKING(expander.setOutput(Pins(portMask)));
	}

	static void
	setInput()
	{
		RF_CALL_BLOCKING(expander.setInput(Pins(portMask)));
	}

	static PortType
	read()
	{
		RF_CALL_BLOCKING(expander.readInput());

		return (expander.getInputs().value & portMask) >> StartIndex;
	}

	static void
	write(PortType data)
	{
		data = (data & dataMask) << StartIndex;
		data = (expander.getOutputs().value & ~portMask) | data;
		RF_CALL_BLOCKING( expander.writePort(data) );
	}

	static void
	toggle()
	{
		RF_CALL_BLOCKING( expander.toggle(Pins(portMask)) );
	}
};

/// @cond
template <
	typename GpioExpander,
	GpioExpander &expander,
	typename GpioExpander::Pin StartPin,
	uint8_t Width >
class GpioExpanderPort<GpioExpander, expander, StartPin, Width, GpioPort::DataOrder::Reversed> : public xpcc::GpioPort
{
	using Pins = typename GpioExpander::Pins;
	using PortType = typename GpioExpander::PortType;

	static constexpr uint8_t StartIndex = GpioExpander::indexFromPin(StartPin);
	static constexpr uint8_t StartIndexReversed = (sizeof(PortType) * 4 - StartIndex - Width) + sizeof(PortType) * 4;

	static_assert(Width <= GpioExpander::width, "Width too large.");
	static_assert(Width > 0, "Width should be at least 1.");
	static_assert(StartIndex + Width <= GpioExpander::width, "StartPin + Width too large.");

	static constexpr PortType dataMask = (1 << Width) - 1;
	static constexpr PortType portMask = dataMask << StartIndex;
	static constexpr PortType portMaskReversed = dataMask << StartIndexReversed;

public:
	static constexpr uint8_t width = Width;

public:
	static void
	setOutput()
	{
		RF_CALL_BLOCKING(expander.setOutput(Pins(portMask)));
	}

	static void
	setInput()
	{
		RF_CALL_BLOCKING(expander.setInput(Pins(portMask)));
	}

	static PortType
	read()
	{
		RF_CALL_BLOCKING(expander.readInput());

		return (xpcc::bitReverse(PortType(expander.getInputs().value & portMask))) >> StartIndexReversed;
	}

	static void
	write(PortType data)
	{
		data = xpcc::bitReverse(PortType((data & dataMask) << StartIndexReversed));
		data = (expander.getOutputs().value & ~portMask) | data;
		RF_CALL_BLOCKING( expander.writePort(data) );
	}

	static void
	toggle()
	{
		RF_CALL_BLOCKING( expander.toggle(Pins(portMask)) );
	}
};
/// @endcond

} // xpcc namespace

#endif // XPCC_IO_EXPANDER_PIN_HPP
