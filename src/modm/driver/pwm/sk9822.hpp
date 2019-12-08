/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "apa102.hpp"

namespace modm
{

/// @ingroup modm_driver_sk9822
template< class SpiMaster, size_t LEDs >
using Sk9822 = Apa102<SpiMaster, LEDs>;

}	// namespace modm
