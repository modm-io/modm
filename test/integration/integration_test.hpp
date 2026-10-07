/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTEGRATION_TEST_HPP
#define MODM_INTEGRATION_TEST_HPP

#include <modm/debug/logger.hpp>

/**
 * Every integration test ends by printing exactly one of these lines:
 *
 *     INTEGRATION TEST PASSED
 *     INTEGRATION TEST FAILED
 *
 * A test runner only needs to wait for this line on the logger of the board.
 * On hosted targets the result is also the exit code of the program.
 *
 * Usage: `return finishTest(passed);` at the end of `main()`.
 */
/// Only prints the result: for tests that must keep their main loop running.
inline void
printTestResult(bool passed)
{
	MODM_LOG_INFO << modm::endl << "INTEGRATION TEST " << (passed ? "PASSED" : "FAILED")
				  << modm::endl << modm::flush;
}

inline int
finishTest(bool passed)
{
	printTestResult(passed);
#ifdef MODM_OS_HOSTED
	return passed ? 0 : 1;
#else
	// keep the interrupts running so that buffered loggers can finish
	while (true) {}
#endif
}

#endif // MODM_INTEGRATION_TEST_HPP
