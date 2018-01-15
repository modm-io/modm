/*
 * Copyright (c) 2013-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GPIO_INVERTED_HPP
#define MODM_GPIO_INVERTED_HPP

#include <modm/architecture/interface/gpio.hpp>

namespace modm
{

namespace platform
{

/**
 * Invert a pin in software.
 *
 * This template can be used the invert the logic level of a normal
 * pin template.
 *
 * Example:
 * @code
 * #include <modm/platform/platform.hpp>
 *
 * typedef GpioInverted< GpioOutputB0 > Led;
 *
 * ...
 * Led::setOutput();
 * Led::reset();
 * @endcode
 *
 * @author	Fabian Greif
 * @ingroup	gpio
 */
template < class Pin >
class GpioInverted : public Pin
{
public:
	using Output = GpioInverted<typename Pin::Output>;
	using Input = GpioInverted<typename Pin::Input>;
	using IO = GpioInverted<typename Pin::IO>;
	using Type = typename Pin::Type;

public:
	modm_always_inline static void
	setOutput()
	{
		Pin::setOutput();
	}

	modm_always_inline static void
	setOutput(bool value)
	{
		Pin::setOutput(!value);
	}

	modm_always_inline static void
	set()
	{
		Pin::reset();
	}

	modm_always_inline static void
	set(bool value)
	{
		Pin::set(!value);
	}

	modm_always_inline static void
	reset()
	{
		Pin::set();
	}

	modm_always_inline static bool
	read()
	{
		return !Pin::read();
	}
};

}	// namespace platform

}	// namespace modm

#endif // MODM_GPIO_INVERTED_HPP
