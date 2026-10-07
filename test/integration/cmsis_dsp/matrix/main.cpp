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

#if __has_include(<modm/board.hpp>)
#include <modm/board.hpp>
#else
#include <modm/platform.hpp>
#include <modm/debug/logger.hpp>
#endif
#include <arm_math.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#define main arm_cmsis_dsp_example
#include "../../../../ext/arm/cmsis-dsp/Examples/ARM/arm_matrix_example/math_helper.h"
#include "../../../../ext/arm/cmsis-dsp/Examples/ARM/arm_matrix_example/math_helper.c"
#define while return status; void
#include "../../../../ext/arm/cmsis-dsp/Examples/ARM/arm_matrix_example/arm_matrix_example_f32.c"
#undef while
#undef main
#pragma GCC diagnostic pop

#define example_name "matrix"
#include "../runner.cpp"
