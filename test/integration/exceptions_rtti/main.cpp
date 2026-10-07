/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <typeinfo>
#include <cstring>

#include "../integration_test.hpp"

/**
 * Tests that C++ exceptions and RTTI work on the microcontroller when the
 * `modm:stdc++:exceptions` and `modm:stdc++:rtti` options are enabled.
 */

struct Base { virtual ~Base() = default; };
struct Derived : Base {};
struct Other : Base {};

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	bool passed = true;

	// RTTI: type names and dynamic casts
	Derived derived;
	Base *const base = &derived;
	MODM_LOG_INFO << "TypeId of base: " << typeid(*base).name() << modm::endl;
	passed &= (typeid(*base) == typeid(Derived));
	passed &= (dynamic_cast<Derived*>(base) != nullptr);
	passed &= (dynamic_cast<Other*>(base) == nullptr);

	// Exceptions: every thrown value must arrive in the handler
	uint32_t caught{0};
	for (uint32_t counter = 0; counter < 10; counter++)
	{
		try {
			throw counter;
		}
		catch (uint32_t code) {
			MODM_LOG_INFO << "Caught exception #" << code << modm::endl;
			if (code == counter) caught++;
		}
	}
	passed &= (caught == 10);

	return finishTest(passed);
}
