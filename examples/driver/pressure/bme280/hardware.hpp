/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
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

#include <modm/board.hpp>

// ----------------------------------------------------------------------------
// Sensors at I2C

using SensorsAI2cMaster = I2cMaster1;
using SensorsBI2cMaster = I2cMaster2;

#endif // ENVIRONMENT_HARDWARE_HPP
