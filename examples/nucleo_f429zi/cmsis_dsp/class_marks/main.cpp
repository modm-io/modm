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
#define std var_std
#define while return ARM_MATH_SUCCESS; void // has no status variable
#include "../../../../ext/arm/cmsis/CMSIS/DSP/Examples/ARM/arm_class_marks_example/arm_class_marks_example_f32.c"
#undef while
#undef main
#undef std

#define example_name "class_marks"
#include "../runner.cpp"
