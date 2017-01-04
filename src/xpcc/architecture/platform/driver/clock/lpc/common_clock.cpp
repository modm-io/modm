/*
 * Copyright (c) 2013-2014, 2016, Niklas Hauser
 * Copyright (c) 2013-2014, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../generic/common_clock.hpp"

#include <modm/architecture/detect.hpp>

namespace xpcc
{

namespace clock
{

uint32_t xpcc_fastdata fcpu(MHz12);
uint32_t xpcc_fastdata fcpu_kHz(12000);
uint16_t xpcc_fastdata fcpu_MHz(12);
uint16_t xpcc_fastdata ns_per_loop(4000/12);

}

}
