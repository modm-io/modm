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

#include <modm/board.hpp>
#include <arm_math.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#define main arm_cmsis_dsp_example
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_linear_interp_example/math_helper.h"
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_linear_interp_example/math_helper.c"
/* `float arm_linear_interep_table[188495]` is 736kB large and MUST be placed in Flash
 * memory NOT SRAM otherwise it won't fit on the device. Since we don't want (=cannot)
 * to change the example, we need to take drastic measures... */
#define float const float
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_linear_interp_example/arm_linear_interp_data.c"
#define while return status; void
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_linear_interp_example/arm_linear_interp_example_f32.c"
#undef while
#undef float
#undef main
#pragma GCC diagnostic pop

#define example_name "linear_interp"
#include "../runner.cpp"
