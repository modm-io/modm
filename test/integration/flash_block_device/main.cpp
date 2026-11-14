/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Two fibers erase, program and read two BdFlash ranges at the end of the flash
// at the same time. On dual-bank devices the flash driver yields during these
// operations, so the fibers interleave and only the lock shared by all BdFlash
// keeps the operations of the one flash controller apart. A third fiber counts
// how often it runs during the operations. Every page is erased only a few times.

#include <modm/board.hpp>
#include <modm/platform/flash/block_device.hpp>
#include <modm/processing.hpp>
#include <cstring>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

constexpr uint32_t Range{8 * 1024};
modm::platform::BdFlash<Flash::Size - 2 * Range, Range> lower;
modm::platform::BdFlash<Flash::Size - Range, Range> upper;

constexpr uint32_t
mix(uint32_t x)
{
	x ^= x >> 16; x *= 0x7feb352d;
	x ^= x >> 15; x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

uint32_t errors{}, done{}, turns{};

/// Programs the range in chunks, the other code of the fiber yields in between
template< size_t Chunk, class Device >
void
stress(Device &device, uint32_t seed)
{
	alignas(8) uint8_t data[Chunk], back[Chunk];
	for (uint32_t round{0}; round < 4; round++)
	{
		if (not device.erase(0, Range)) errors++;
		for (uint32_t addr{0}; addr < Range; addr += Chunk)
		{
			for (size_t ii{0}; ii < Chunk; ii++) data[ii] = mix(seed ^ (round << 24) ^ (addr + ii));
			if (not device.program(data, addr, Chunk)) errors++;
			if (not device.read(back, addr, Chunk) or std::memcmp(data, back, Chunk)) errors++;
			modm::this_fiber::yield();
		}
		// everything again, after the other fiber programmed its range
		for (uint32_t addr{0}; addr < Range; addr += Chunk)
		{
			for (size_t ii{0}; ii < Chunk; ii++) data[ii] = mix(seed ^ (round << 24) ^ (addr + ii));
			if (not device.read(back, addr, Chunk) or std::memcmp(data, back, Chunk)) errors++;
		}
	}
	done++;
}

// Different chunks, so that the fibers do not run in lockstep
modm::Fiber<> fiberLower([] { stress<64>(lower, 0x1234); });
modm::Fiber<> fiberUpper([] { stress<32>(upper, 0xabcd); });
modm::Fiber<> counter([] { while (done < 2) { turns++; modm::this_fiber::yield(); } });
modm::Fiber<> result([]
{
	while (done < 2) modm::this_fiber::sleep_for(10ms);
	MODM_LOG_INFO << "BdFlash from two fibers: " << errors << " errors, " << turns
				  << " turns of another fiber" << modm::endl;
	printTestResult(errors == 0 and lower.initialize() and upper.initialize());
});

int
main()
{
	Board::initialize();
	modm::delay(1s); // let RTT attach before the flash stalls the bus
	modm::fiber::Scheduler::run();
	return 0;
}
