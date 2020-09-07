/*
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2018, Fabian Greif
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


namespace modm
{

namespace platform
{

template< int Pin >
class GpioPin : public Gpio, public modm::GpioIO
{
	static Gpio::InputType inputType;
public:
	using Output = GpioPin<Pin>;
	using Input = GpioPin<Pin>;
	using IO = GpioPin<Pin>;
	using Type = GpioPin<Pin>;

public:
	inline static void configure(Gpio::InputType type) {}
	modm_always_inline static void setInput() {
    pinMode(Pin, INPUT);
  }
	inline static void setInput(Gpio::InputType type) {
    setInput();
  }
	modm_always_inline static void setOutput() {
    pinMode(Pin, OUTPUT);
  }
	modm_always_inline static void setOutput(OutputType) {
    setOutput();
  }
	modm_always_inline static void setOutput(bool status) {
    setOutput();
		set(status);
	}
	inline static void set() {
    digitalWrite(Pin, HIGH);
	}
	modm_always_inline static void reset() {
    digitalWrite(Pin, LOW);
	}
	inline static void set(bool status) {
    digitalWrite(Pin, status);
	}
	inline static bool isSet() {
    return digitalRead(Pin);
	}
  inline static void toggle() {
		if (isSet()) { set(); }
		else { reset(); }
  }
	modm_always_inline static modm::Gpio::Direction getDirection() {
		return modm::Gpio::Direction::Out;
	}
};

/// @}

}	// namespace platform

}	// namespace modm

