// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "gpio.hpp"

extern "C"
void
xpcc_gpio_enable(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_GPIO;
}
