/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_HPP
#define MODM_INTERFACE_HPP

#include <stdint.h>

/**
 * @ingroup 	architecture
 * @defgroup	interface	Architecture Interfaces
 *
 * All hardware peripherals with common interfaces.
 */

#include "interface/adc_interrupt.hpp"
#include "interface/adc.hpp"
#include "interface/assert.hpp"
#include "interface/assert.hpp"
#include "interface/atomic_lock.hpp"
#include "interface/can_message.hpp"
#include "interface/can.hpp"
#include "interface/clock.hpp"
#include "interface/delay.hpp"
#include "interface/gpio_expander.hpp"
#include "interface/gpio.hpp"
#include "interface/i2c_device.hpp"
#include "interface/i2c_master.hpp"
#include "interface/i2c_transaction.hpp"
#include "interface/i2c.hpp"
#include "interface/interrupt.hpp"
#include "interface/memory.hpp"
#include "interface/one_wire.hpp"
#include "interface/peripheral.hpp"
#include "interface/register.hpp"
#include "interface/spi_device.hpp"
#include "interface/spi.hpp"
#include "interface/uart.hpp"

#endif	// MODM_INTERFACE_HPP
