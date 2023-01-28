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
#define while return index; void
#include "../../../../ext/arm/cmsis-dsp/CMSIS-DSP/Examples/ARM/arm_bayes_example/arm_bayes_example_f32.c"
#undef while
#undef main

#define example_name "bayes"
#include "../runner.cpp"
