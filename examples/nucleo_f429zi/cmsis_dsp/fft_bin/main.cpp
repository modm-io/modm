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

#define main arm_cmsis_dsp_example
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_fft_bin_example/arm_fft_bin_data.c"
#define while return status; void
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_fft_bin_example/arm_fft_bin_example_f32.c"
#undef while
#undef main

#define example_name "fft_bin"
#include "../runner.cpp"
