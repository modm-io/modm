/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef ENVIRONMENT_HARDWARE_HPP
#define ENVIRONMENT_HARDWARE_HPP

#include <xpcc/architecture/platform.hpp>

// ----------------------------------------------------------------------------
// Sensors at I2C

using SensorsAI2cMaster = xpcc::stm32::I2cMaster1;
using SensorsBI2cMaster = xpcc::stm32::I2cMaster2;

#endif // ENVIRONMENT_HARDWARE_HPP
