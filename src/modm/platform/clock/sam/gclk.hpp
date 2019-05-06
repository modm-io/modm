/*
 * Copyright (c) 2019, Ethan Slattery
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include "../device.hpp"

namespace modm::platform
{

/**
 * Clock management
 *
 * \ingroup modm_platform_gclk
 */
class GenericClockController
{
public:
    template< uint32_t Core_Hz >
	void
	updateCoreFrequency();
};

}

#include "gclk_impl.hpp"
