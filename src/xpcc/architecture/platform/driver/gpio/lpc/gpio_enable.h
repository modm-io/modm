// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
#ifndef XPCC_LPC_GPIO_ENABLE_H
#define XPCC_LPC_GPIO_ENABLE_H

#include "../../../device.hpp"

// TODO: Move to common file
#define SYSAHBCLKCTRL_GPIO		(1<<6)
#define IOCON_ADMODE_DIGITAL	(1<<7)

static void
xpcc_gpio_enable()
{
	LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_GPIO;
}

#endif // XPCC_LPC_GPIO_ENABLE_H
