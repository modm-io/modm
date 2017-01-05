/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/architecture/driver/accessor.hpp>
#include <modm/container/smart_pointer.hpp>

using namespace Board;

uint8_t buffer[300] = {0};

uint32_t input = 0x06050403;
uint32_t output;

void error()
{
	while(1)
	{
		LedUp::toggle();
		modm::delayMilliseconds(100);
	}
}

// ----------------------------------------------------------------------------
int
main()
{
	LedRight::setOutput(modm::Gpio::High);
	LedLeft::setOutput();
	LedUp::setOutput();
	LedDown::setOutput();

	uint8_t offset(0);

	// if an unaligned access is trapped, the blue led will blink

	while (1)
	{
		{
			// "smart" pointer is created on the heap
			modm::SmartPointer p(&input);

			// manually interpreting the pointer
			output = *reinterpret_cast<uint32_t*>(p.getPointer());
			if (output != input) error();

			// letting the class interpret it
			output = p.get<uint32_t>();
			if (output != input) error();
		}

		{
			// `u32` is a pointer to a modm::unaligned_t<uint32_t> !
			auto *u32 = modm::asUnaligned<uint32_t>(buffer + offset);
			// this is short for:
			// modm::unaligned_t<uint32_t> *u32 = reinterpret_cast<modm::unaligned_t<uint32_t>*>(buffer + offset);
			// write to the unaligned location
			*u32 = input;
			// read from the unaligned location
			output = *u32;
			if (output != input) error();

			// Anonymous form
			*modm::asUnaligned<uint32_t>(buffer + offset) = input;
			output = *modm::asUnaligned<uint32_t>(buffer + offset);
			if (output != input) error();
		}

		input += 0x0125197;
		offset += 1;

		LedRight::toggle();
		LedLeft::toggle();
		modm::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
