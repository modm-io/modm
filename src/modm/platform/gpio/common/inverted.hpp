/*
 * Copyright (c) 2017-2018, Niklas Hauser
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
 * using Led = GpioInverted<GpioOutputB0>;
 * Led::setOutput();
 * Led::reset(); // -> maps to GpioOutputB0::set()
 * @endcode
 *
 * @author	Fabian Greif
 * @ingroup	modm_platform_gpio
 */
template < class Pin >
class GpioInverted : public Pin
{
public:
	using Output = GpioInverted<typename Pin::Output>;
	using Input = GpioInverted<typename Pin::Input>;
	using IO = GpioInverted<typename Pin::IO>;
	using Type = typename Pin::Type;
	using Data = typename Pin::Data;
	static constexpr bool isInverted = not Pin::isInverted;

public:
	using Pin::setOutput;
	using Pin::set;
	using Pin::reset;
	using Pin::read;
	using Pin::isSet;

	inline static void
	setOutput(bool value)
	{
		Pin::setOutput(!value);
	}

	inline static void
	set()
	{
		Pin::reset();
	}

	inline static void
	set(bool value)
	{
		Pin::set(!value);
	}

	inline static void
	reset()
	{
		Pin::set();
	}

	inline static bool
	read()
	{
		return !Pin::read();
	}

	inline static bool
	isSet()
	{
		return !Pin::isSet();
	}
};

}	// namespace platform

}	// namespace modm

#endif // MODM_GPIO_INVERTED_HPP
