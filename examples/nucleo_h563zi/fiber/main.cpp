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
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>

using namespace Board;
using namespace std::chrono_literals;

namespace
{

constexpr uint32_t YieldGoal = 2'000'000;
volatile uint32_t yieldCounter = 0;
volatile uint32_t ringCounter = 0;

void
yield_worker()
{
	while (true)
	{
		++yieldCounter;
		modm::this_fiber::yield();
	}
}

extern modm::Fiber<512> fiberB;
extern modm::Fiber<512> fiberC;

modm_faststack modm::Fiber<768> fiberA([]{
	++ringCounter;
	if ((ringCounter & 0x3ff) == 0)
	{
		const auto ring = uint32_t(ringCounter);
		const auto yields = uint32_t(yieldCounter);
		MODM_LOG_INFO << "ring=" << ring
				<< " yields=" << yields
				<< " A=" << fiberA.stack_usage()
				<< modm::endl;
	}
	modm::this_fiber::yield();
	fiberB.start();
});

modm_faststack modm::Fiber<512> fiberB([]{
	++ringCounter;
	modm::this_fiber::yield();
	fiberC.start();
}, modm::fiber::Start::Later);

modm_faststack modm::Fiber<512> fiberC([]{
	++ringCounter;
	modm::this_fiber::yield();
	fiberA.start();
}, modm::fiber::Start::Later);

modm_faststack modm::Fiber<768> fiberYield(yield_worker);

modm_faststack modm::Fiber<1024> fiberReporter([]{
	while (true)
	{
		modm::this_fiber::sleep_for(250ms);
		Leds::toggle();
		const auto yields = uint32_t(yieldCounter);
		const auto ring = uint32_t(ringCounter);
		MODM_LOG_INFO << "report yields=" << yields
				<< " ring=" << ring
				<< " WY=" << fiberYield.stack_usage()
				<< " WA=" << fiberA.stack_usage()
				<< " WB=" << fiberB.stack_usage()
				<< " WC=" << fiberC.stack_usage()
				<< modm::endl;
	}
});

}

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "H563ZI fiber stress example" << modm::endl;
	MODM_LOG_INFO << "Exercises repeated yield and restart handover on ARMv8-M" << modm::endl;
	MODM_LOG_INFO.flush();

	modm::fiber::Scheduler::run(modm::fiber::Scheduler::AutoWatermark);
	return 0;
}