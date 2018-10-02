/*
 * Copyright (c) 2018 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------


#ifndef MODM_INVENSENSE_EMD_HPP
#define MODM_INVENSENSE_EMD_HPP

#include <modm/architecture/interface/i2c_transaction.hpp>

namespace modm::emd
{
using start_t = bool(*)(modm::I2cTransaction*);

bool set_i2c_start_function(start_t start);
}

#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"

extern "C" {
#include "inv_mpu.h"
}

#endif // MODM_INVENSENSE_EMD_HPP