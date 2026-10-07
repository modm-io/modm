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

#include "../integration_test.hpp"

// Runs one of the examples of CMSIS-DSP, which check their own results.
int main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();

	const uint32_t start{DWT->CYCCNT};
	const int status = arm_cmsis_dsp_example();
	const uint32_t diff{DWT->CYCCNT - start};

	MODM_LOG_INFO << "Example '" << example_name << "' took ~"
				  << (diff / modm::platform::delay_fcpu_MHz) << "us" << modm::endl;
#else
	const int status = arm_cmsis_dsp_example();
	MODM_LOG_INFO << "Example '" << example_name << "'" << modm::endl;
#endif

	return finishTest(status != ARM_MATH_TEST_FAILURE);
}
