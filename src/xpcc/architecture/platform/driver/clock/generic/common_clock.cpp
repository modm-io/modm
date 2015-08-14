// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "common_clock.hpp"

namespace xpcc
{

namespace clock
{

uint32_t ATTRIBUTE_FASTDATA fcpu(MHz8);
uint32_t ATTRIBUTE_FASTDATA fcpu_kHz(8000);
uint16_t ATTRIBUTE_FASTDATA fcpu_MHz(8);
uint16_t ATTRIBUTE_FASTDATA ns_cycle_pre asm("ns_per_cycle") (125*9);

}

}

