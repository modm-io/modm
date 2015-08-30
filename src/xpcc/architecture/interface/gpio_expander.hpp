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
#include <xpcc/processing/resumable.hpp>

namespace xpcc
{

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
