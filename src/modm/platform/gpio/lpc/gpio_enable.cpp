/*
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "gpio.hpp"

extern "C"
void
modm_gpio_enable(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_GPIO;
}
