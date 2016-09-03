// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "../generic/common_clock.hpp"

#include <xpcc/architecture/detect.hpp>

namespace xpcc
{

namespace clock
{

uint32_t ATTRIBUTE_FASTDATA fcpu(MHz12);
uint32_t ATTRIBUTE_FASTDATA fcpu_kHz(12000);
uint16_t ATTRIBUTE_FASTDATA fcpu_MHz(12);
uint16_t ATTRIBUTE_FASTDATA ns_per_loop(4000/12);

}

}
