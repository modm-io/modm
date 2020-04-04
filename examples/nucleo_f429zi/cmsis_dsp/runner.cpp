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

int main()
{
	Board::initialize();

	const uint32_t start{DWT->CYCCNT};
	const int status = arm_cmsis_dsp_example();
	const uint32_t diff{DWT->CYCCNT - start};

	if (status != ARM_MATH_TEST_FAILURE) {
		MODM_LOG_INFO << "Example '" << example_name << "' passed in ~" << (diff / modm::platform::delay_fcpu_MHz) << "us!" << modm::endl;
	} else {
		MODM_LOG_ERROR << "Example '" << example_name << "' failed!" << modm::endl;
	}

	while(1) {
		if (status != ARM_MATH_TEST_FAILURE) {
			Board::LedBlue::toggle();
		} else {
			Board::LedRed::toggle();
		}
		modm::delay(1s);
	}

	return 0;
}
