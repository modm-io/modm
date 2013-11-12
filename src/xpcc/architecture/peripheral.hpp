// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_HPP
#define XPCC_PERIPHERAL_HPP

#include <stdint.h>
#include <cstddef>
#include <xpcc/math/tolerance.hpp>

/**
 * @ingroup 	architecture
 * @defgroup	peripheral	Peripherals
 *
 * All hardware peripherals with common interfaces.
 */

namespace xpcc
{

/**
 * Peripheral class
 *
 * This class acts as a base class for all classes describing the
 * public interface of common peripheral drivers.
 * As there is no implementation given, the classes specific to the platform
 * inherit from their respective base classes and must shadow the methods of
 * them.
 *
 * The inheritance is only visible for the documenation, It is completely
 * removed during compile time keeping a possible error at platform level.
 * This is safe, because only one platform can be compiled at once.
 *
 * This way, no virtual functions are needed and no overhead is generated,
 * but we still have clean inheritance in the documentation.
 * There is no need to document the platform specific implementation, since
 * it is taken from the base class.
 *
 * @ingroup peripheral
 * @author	Niklas Hauser
 */
class Peripheral
{
#ifdef __DOXYGEN__
	/// initializes the peripheral, must be called before use.
	static void
	initialize();

	/// configures a peripheral for a specific purpose
	static void
	configurePurpose();

	/// sets a parameter
	static void
	setParameter();

	/// returns a parameter
	static void
	getParameter();

	/// Read an interrupt flag.
	static bool
	getInterruptFlag();

	/**
	 * Acknowledge an interrupt flag.
	 *
	 * We use acknowledge here, since it describes the intention rather
	 * than the actual implementation.
	 */
	static void
	acknowledgeInterruptFlag();
#endif
};

} // namespace xpcc

#include "peripheral/gpio.hpp"
#include "peripheral/uart.hpp"
#include "peripheral/one_wire.hpp"
#include "peripheral/spi.hpp"
#include "peripheral/i2c.hpp"

#endif	// XPCC_PERIPHERAL_HPP
