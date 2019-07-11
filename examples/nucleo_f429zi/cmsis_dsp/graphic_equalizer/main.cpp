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
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_graphic_equalizer_example/math_helper.h"
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_graphic_equalizer_example/math_helper.c"
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_graphic_equalizer_example/arm_graphic_equalizer_data.c"
#define while return status; void
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_graphic_equalizer_example/arm_graphic_equalizer_example_q31.c"
#undef while
#undef main
#pragma GCC diagnostic pop

#define example_name "graphic_equalizer"
#include "../runner.cpp"
