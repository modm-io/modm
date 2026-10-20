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

// Stresses the SysTick clocks in the window between the counter reload and the
// execution of the SysTick interrupt: first by calling now() in a tight loop,
// which hits the interrupt entry latency, then by calling now() with masked
// interrupts for up to one SysTick period. In both cases the clocks must not
// run backwards and must measure the masked time correctly.

#include <modm/board.hpp>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

static uint32_t now_us() { return modm::chrono::micro_clock::now().time_since_epoch().count(); }
static uint32_t now_ms() { return modm::chrono::milli_clock::now().time_since_epoch().count(); }

int
main()
{
	Board::initialize();
	bool passed = true;

#if __has_include(<modm/platform/clock/systick_timer.hpp>)
	using modm::platform::SysTickTimer;
	const uint32_t period_us = (uint64_t(SysTick->LOAD + 1) * SysTickTimer::us_per_Ncycles) >> SysTickTimer::Ncycles;
#else
	// The clock does not depend on an interrupt, so any period works
	const uint32_t period_us = 10'000;
#endif
	MODM_LOG_INFO << "Clock period " << period_us << "us" << modm::endl;

	// The clocks must not run backwards during the interrupt entry latency
	{
		uint32_t calls{0}, us_backwards{0}, ms_backwards{0};
		uint32_t us_last{now_us()}, ms_last{now_ms()};
		const uint32_t ms_start{ms_last};
		while (ms_last - ms_start < 3000)
		{
			const uint32_t us = now_us();
			if (int32_t(us - us_last) < 0) us_backwards++;
			us_last = us;
			const uint32_t ms = now_ms();
			if (int32_t(ms - ms_last) < 0) ms_backwards++;
			ms_last = ms;
			calls++;
		}
		MODM_LOG_INFO << "Called now() " << calls << " times, clocks ran backwards "
					  << us_backwards << "x (us) and " << ms_backwards << "x (ms)" << modm::endl;
		passed &= (us_backwards == 0) and (ms_backwards == 0);
	}

	// The clocks must measure the time correctly while the interrupts are
	// masked for less than one SysTick period, since the interrupt only
	// latches once.
	for (const uint32_t eighths : {2, 4, 6, 7})
	{
		const uint32_t masked_us = period_us * eighths / 8;
		uint32_t failures{0};
		for (uint32_t ii = 0; ii < 16; ii++)
		{
			// vary the phase to the SysTick reload
			modm::delay_us((ii * 997) % period_us);
			const uint32_t us0 = now_us(), ms0 = now_ms();
			__disable_irq();
			modm::delay_us(masked_us);
			const uint32_t us1 = now_us(), ms1 = now_ms();
			__enable_irq();
			const uint32_t us2 = now_us(), ms2 = now_ms();

			const uint32_t us_diff = us1 - us0;
			const uint32_t ms_diff = ms1 - ms0;
			if (us_diff < masked_us or us_diff > masked_us + masked_us / 8 + 100 or
				ms_diff + 1 < masked_us / 1000 or ms_diff > us_diff / 1000 + 1 or
				int32_t(us2 - us1) < 0 or int32_t(ms2 - ms1) < 0)
			{
				MODM_LOG_ERROR << "Masked " << masked_us << "us, but measured "
							   << us_diff << "us and " << ms_diff << "ms" << modm::endl;
				failures++;
			}
		}
		MODM_LOG_INFO << "Masked interrupts for " << masked_us << "us: "
					  << failures << " failures" << modm::endl;
		passed &= (failures == 0);
	}

	return finishTest(passed);
}
