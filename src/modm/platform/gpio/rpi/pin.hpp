/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/gpio.hpp>
#include <wiringPi.h>

#include "base.hpp"


namespace modm::platform
{

template< int Pin >
class GpioPin : public Gpio, public modm::GpioIO
{
	static inline bool output{false};
public:
	using Output = GpioPin<Pin>;
	using Input = GpioPin<Pin>;
	using IO = GpioPin<Pin>;
	using Type = GpioPin<Pin>;

public:
	inline static void setOutput() { pinMode(Pin, OUTPUT);}
	inline static void setOutput(OutputType) { setOutput(); }
	inline static void setOutput(bool status)
	{
		setOutput();
		set(status);
	}

	inline static void set() { set(true); }
	inline static void reset() { set(false); }
	inline static bool isSet() { return output; }
	inline static void set(bool status) { digitalWrite(Pin, status); output = status; }
	inline static void toggle()
	{
		if (isSet()) { set(); }
		else { reset(); }
	}

	inline static void setInput() { pinMode(Pin, INPUT); }
	inline static void setInput(Gpio::InputType type) { setInput(); configure(type); }
	inline static void configure(Gpio::InputType type) { pullUpDnControl(Pin, int(type)); }

	inline static bool read() { return digitalRead(Pin); }

	inline static modm::Gpio::Direction getDirection()
	{ return modm::Gpio::Direction::InOut; }

public:
	struct BitBang {}
};

}   // namespace modm::platform

