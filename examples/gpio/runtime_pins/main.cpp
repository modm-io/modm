/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <array>

using namespace std::chrono_literals;
using namespace modm::platform;

// Runtime pins are values: they can be copied, stored in arrays and selected
// by an index.
std::array<modm::AnyGpio, 3> leds{Board::LedGreen{}, Board::LedYellow{}, Board::LedRed{}};

int
main()
{
	Board::initialize();
	for (auto led : leds) led.setOutput(modm::Gpio::Low);

	// A pin can also be chosen by port and number at runtime: PB0 is the green LED
	const GpioPin green{Gpio::Port::B, 0};
	green.set();
	modm::delay(1s);
	green.reset();

	MODM_LOG_INFO << "Running light, press the button to reverse it." << modm::endl;
	size_t index{0};
	int direction{1};
	bool pressed{false};
	while (true)
	{
		const auto led = leds[index];
		led.set();
		modm::delay(150ms);
		led.reset();

		if (Board::Button::read() and not pressed)
		{
			direction = -direction;
			MODM_LOG_INFO << "Direction " << (direction > 0 ? "up" : "down") << modm::endl;
		}
		pressed = Board::Button::read();
		index = (index + leds.size() + direction) % leds.size();
	}
}
