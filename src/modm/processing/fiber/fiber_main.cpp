/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "fiber.hpp"

modm_extern_c int
main(void);

modm::fiber::Stack<2048> main_stack;
modm::Fiber main_fiber(main_stack, &main);

modm_extern_c void
__modm_fiber_initialize()
{
	modm::fiber::scheduler.start();
}
