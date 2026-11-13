/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Creates flash words with a double ECC error by resetting the device during
// programming, then checks that Flash::read() and Flash::program() return an
// error for them instead of faulting. The reset comes from a timer interrupt
// while the last page is programmed, which must not be in the bank of the
// program image, so that the vector table and handler may stay in flash.

#include <modm/board.hpp>
#include <modm/platform/flash/flash.hpp>
#include <cstring>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

namespace
{

constexpr uint32_t Magic = 0xECCDECCD;
constexpr uint32_t Steps = 300;
constexpr uint32_t Wanted = 3;

struct State { uint32_t magic, index, step, ticks, found, errors; };
modm_section(".noinit") State state;

uint16_t page() { return Flash::getPage(Flash::OriginAddr + Flash::Size - 1); }
uintptr_t address(uint32_t index) { return uintptr_t(Flash::getAddr(page())) + index * Flash::WriteSize; }

/// Programs the word at index and resets after delay timer ticks, 0 measures the time
uint32_t
programWithReset(uint32_t index, uint32_t delay)
{
	Flash::MaxWordType word;
	std::memset(&word, 0x5a, sizeof(word));
	TIM2->CR1 = 0;
	// No update event, its UIF flag would trigger the reset at once
	TIM2->PSC = 0;
	TIM2->ARR = delay ? delay : 0xffff'ffff;
	TIM2->CNT = 0;
	TIM2->SR = 0;
	TIM2->DIER = delay ? TIM_DIER_UIE : 0;
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;
	Flash::program(address(index), word);
	const uint32_t ticks = TIM2->CNT;
	if (delay) while (true) ;
	return ticks;
}

} // namespace

MODM_ISR(TIM2)
{
	NVIC_SystemReset();
}

int
main()
{
	Board::initialize();
	Rcc::enable<Peripheral::Tim2>();
	Flash::unlock();
	const size_t words = Flash::getSize(page()) / Flash::WriteSize;

	if (state.magic != Magic)
	{
		const bool rom = SCB->VTOR >= Flash::OriginAddr and SCB->VTOR < Flash::OriginAddr + Flash::Size;
		MODM_LOG_INFO << "\n\nFlash ECC test, vector table in " << (rom ? "flash" : "RAM") << modm::endl;
		state = {Magic, 1, 0, 0, 0, 0};
		Flash::erase(page());
		state.ticks = programWithReset(1, 0);
		Flash::erase(page());
	}
	else
	{
		// The word programmed before the reset
		uint8_t data[Flash::WriteSize];
		const uint32_t err = Flash::read(address(state.index), data, sizeof(data));
		uint32_t next = 2;
		if (err)
		{
			// The buffer program must refuse it instead of faulting or programming it
			const uint32_t perr = Flash::program(address(state.index), data, sizeof(data));
			MODM_LOG_INFO << "Double ECC error at " << (void *) address(state.index) << ": read returned "
						  << err << ", program returned " << perr << modm::endl;
			if (perr == 0) state.errors++;
			// The erased neighbours must stay programmable, even if prefetches or
			// cache line fills read the damaged word too
			std::memset(data, 0xa5, sizeof(data));
			for (const uint32_t neighbour : {state.index - 1, state.index + 1})
			{
				uint8_t back[Flash::WriteSize];
				const uint32_t nerr = Flash::program(address(neighbour), data, sizeof(data));
				if (nerr or Flash::read(address(neighbour), back, sizeof(back)) or std::memcmp(back, data, sizeof(back)))
				{
					MODM_LOG_ERROR << "Programming the neighbour at " << (void *) address(neighbour)
								   << " failed with " << nerr << modm::endl;
					state.errors++;
				}
			}
			state.found++;
			next = 4;
		}
		// Every other word, so that both neighbours of a damaged word are erased
		if ((state.index += next) >= words - 1)
		{
			Flash::erase(page());
			state.index = 1;
		}
		state.step++;
	}

	if (state.found >= Wanted or state.step >= Steps)
	{
		Flash::erase(page());
		MODM_LOG_INFO << state.found << " double ECC errors in " << state.step << " resets" << modm::endl;
		const bool passed = state.found and not state.errors;
		state.magic = 0;
		finishTest(passed);
	}
	// Sweep the reset over the program time
	modm::delay(5ms);
#if __DCACHE_PRESENT
	SCB_CleanDCache();
#endif
	programWithReset(state.index, 1 + (state.ticks * (state.step % 100)) / 90);
	return 0;
}
