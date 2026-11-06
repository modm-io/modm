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

#include <modm/board.hpp>
#include <modm/driver/logic_analyzer.hpp>

/**
 * Samples PA0 and PB0 at 1 MHz and keeps the capture in RAM, where
 * `modm_tools.logic_analyzer` can read it out over SWD:
 *
 *     PYTHONPATH=modm python3 -m modm_tools.logic_analyzer --elf build/scons-release/project.elf \
 *         --config modm/openocd.cfg --channels A0,B0 --vcd capture.vcd
 *
 * Connect the signals you want to observe to PA0 and/or PB0. Both pins are
 * inputs by default; the driver only reads the input data register.
 */

using namespace modm::platform;

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "Logic analyzer example" << modm::endl;

	const uint32_t rate =
		modm::LogicAnalyzer::start<Board::SystemClock, 1'000'000, GpioA0, GpioB0>();
	MODM_LOG_INFO << "Sampling at " << rate << " Hz into " << modm::LogicAnalyzer::Samples
				  << " samples" << modm::endl;

	while (modm::LogicAnalyzer::isRunning()) {}

	modm::LogicAnalyzer::stop();
	MODM_LOG_INFO << "Captured " << modm::LogicAnalyzer::count(0)
				  << " samples, read them out with modm_tools.logic_analyzer" << modm::endl;

	// Keep the capture in RAM for the debugger.
	while (true) {}
}
