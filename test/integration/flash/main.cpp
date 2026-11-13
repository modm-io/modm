/*
 * Copyright (c) 2020, 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Checks the page geometry of the internal flash, then erases and programs
// random pages behind the program image with random data and densities, and
// verifies the programmed page and another page programmed earlier, which
// catches erase and program operations that hit the wrong page or bank.
// A second fiber counts how often it runs during erases, which it only does
// on dual-bank devices while the bank without the program image is erased,
// and between units while programming buffers.

#include <modm/board.hpp>
#include <modm/platform/flash/flash.hpp>
#include <modm/processing/fiber.hpp>
#include <cstring>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

#ifndef FLASH_STRESS_SECONDS
#define FLASH_STRESS_SECONDS 30
#endif
// Limits the wear of the flash per run
#ifndef FLASH_MAX_ERASES
#define FLASH_MAX_ERASES 20
#endif

extern "C" const uint8_t __rom_start[], __rom_end[];

namespace
{

using Word = Flash::MaxWordType;
constexpr size_t WordSize = sizeof(Word);
// Small devices have little RAM, larger ones test every n-th page
constexpr uint16_t MaxPages = 256;

struct Slot
{
	uint32_t seed;
	uint16_t erases;
	uint8_t density; // in quarters, 0 = not programmed since the last erase
};
Slot slots[MaxPages];
uint16_t page_begin, slot_count, page_stride;

uint16_t page(uint16_t slot) { return page_begin + slot * page_stride; }

volatile uint32_t ticks{0};
uint32_t rng_state{0x12345678};
uint32_t
mix(uint32_t x)
{
	x ^= x >> 16; x *= 0x7feb352d;
	x ^= x >> 15; x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}
uint32_t random() { return rng_state = mix(rng_state + 0x9e3779b9); }

/// The word at index of a page with seed, and whether it is programmed at all
bool
expected(uint32_t seed, uint8_t density, uint32_t index, Word &word)
{
	uint32_t buffer[(WordSize + 3) / 4];
	for (size_t ii{0}; ii < sizeof(buffer) / 4; ii++)
		buffer[ii] = mix(seed ^ mix(index * 8 + ii));
	std::memcpy(&word, buffer, WordSize);
	return (mix(seed + index) & 3) < density;
}

struct
{
	uint32_t iterations, erases, words, bytes;
	uint32_t erase_errors, program_errors, mismatches;
	uint32_t erase_us, program_us;
	uint32_t buffer_bytes, buffer_us;
	uint32_t read_errors;
	uint32_t yields, buffer_yields;
} stats;

void
mismatch(const char *what, uint16_t page, size_t offset, const uint8_t *actual, const uint8_t *expected)
{
	if (stats.mismatches++ < 5)
	{
		MODM_LOG_ERROR << what << " mismatch in page " << page << " at offset " << offset << ": ";
		for (size_t ii{0}; ii < WordSize; ii++) MODM_LOG_ERROR.printf("%02x", actual[ii]);
		MODM_LOG_ERROR << " != ";
		for (size_t ii{0}; ii < WordSize; ii++) MODM_LOG_ERROR.printf("%02x", expected[ii]);
		MODM_LOG_ERROR << modm::endl;
	}
}

void
verify(uint16_t slot)
{
	const Slot &p = slots[slot];
	const uint16_t page = ::page(slot);
	const uint8_t *const base = Flash::getAddr(page);
	const size_t size = Flash::getSize(page);
	Word word;
	uint8_t erased[WordSize];
	std::memset(erased, Flash::ErasedValue, WordSize);
	uint8_t actual[WordSize];
	for (size_t offset{0}; offset < size; offset += WordSize)
	{
		const bool programmed = p.density and expected(p.seed, p.density, offset / WordSize, word);
		const uint8_t *want = programmed ? reinterpret_cast<const uint8_t*>(&word) : erased;
		if (const uint32_t err = Flash::read(uintptr_t(base + offset), actual, WordSize);
			err and stats.read_errors++ < 5)
		{
			MODM_LOG_ERROR << "Reading page " << page << " at offset " << offset
						   << " failed with " << err << modm::endl;
		}
		if (std::memcmp(actual, want, WordSize))
			return mismatch(programmed ? "Programmed" : "Erased", page, offset, actual, want);
	}
}

void
geometry(uint32_t &errors)
{
	// The banks may not be continuous, so sum up the page sizes
	uint16_t count{0};
	uint32_t end{0}, total{0};
	for (uint16_t page{0}; total < Flash::Size; page++, count++)
	{
		const uint32_t offset = Flash::getOffset(page);
		const size_t size = Flash::getSize(page);
		if (offset < end or size < Flash::MinPageSize or size > Flash::MaxPageSize or
			Flash::getPage(Flash::OriginAddr + offset) != page or
			Flash::getPage(Flash::OriginAddr + offset + size - 1) != page or
			Flash::getAddr(page) != Flash::Origin + offset)
		{
			MODM_LOG_ERROR << "Geometry error at page " << page << " offset " << offset
						   << " size " << size << modm::endl;
			errors++;
		}
		if (count < 4 or total + size >= Flash::Size or size != Flash::getSize(page - 1) or offset != end) {
			MODM_LOG_INFO << "Page " << page << " at " << Flash::getAddr(page) << " size " << size << modm::endl;
		}
		end = offset + size;
		total += size;
	}
	if (total != Flash::Size) {
		MODM_LOG_ERROR << "Pages sum up to " << total << " instead of " << Flash::Size << modm::endl;
		errors++;
	}
	MODM_LOG_INFO << count << " pages, " << Flash::Size << " bytes" << modm::endl;
	// The F7 runs from the read-only ITCM alias of the flash
	const uintptr_t rom_end = Flash::OriginAddr + (__rom_end - __rom_start);
	page_begin = Flash::getPage(rom_end + Flash::getSize(Flash::getPage(rom_end)) - 1);
	if (page_begin >= count) return;
	page_stride = (count - page_begin + MaxPages - 1) / MaxPages;
	slot_count = (count - page_begin + page_stride - 1) / page_stride;
	MODM_LOG_INFO << "Testing every " << page_stride << ". page of [" << page_begin << ", " << count
				  << ") after the image end at " << __rom_end << modm::endl;
}

bool
iteration()
{
	// Pick a page that has not been erased too often yet
	uint16_t slot = random() % slot_count;
	for (uint16_t ii{0}; slots[slot].erases >= FLASH_MAX_ERASES; slot = (slot + 1) % slot_count)
		if (++ii > slot_count) return false;
	Slot &p = slots[slot];
	const uint16_t page = ::page(slot);
	const size_t size = Flash::getSize(page);

	auto start = modm::PreciseClock::now();
	const uint32_t ticks_before = ticks;
	if (const uint32_t err = Flash::erase(page); err and stats.erase_errors++ < 5) {
		MODM_LOG_ERROR << "Erasing page " << page << " failed with " << err << modm::endl;
	}
	stats.erase_us += (modm::PreciseClock::now() - start).count();
	stats.yields += ticks - ticks_before;
	stats.erases++;
	p.erases++;
	p.density = 0;
	verify(slot);

	// Mostly full pages, sometimes sparse ones
	p.density = std::min<uint32_t>(4, 1 + random() % 6);
	p.seed = random();
	start = modm::PreciseClock::now();
	Word word;
	if (p.density == 4)
	{
		// Full pages are programmed in chunks, which may use faster programming modes
		alignas(4) uint8_t chunk[256];
		// The L0 and L1 have 128 byte pages
		const size_t length = std::min(sizeof(chunk), size);
		const uint32_t buffer_ticks = ticks;
		for (size_t offset{0}; offset < size; offset += length)
		{
			for (size_t ii{0}; ii < length; ii += WordSize)
			{
				expected(p.seed, p.density, (offset + ii) / WordSize, word);
				std::memcpy(chunk + ii, &word, WordSize);
			}
			const uint32_t err = Flash::program(uintptr_t(Flash::getAddr(page) + offset), chunk, length);
			if (err and stats.program_errors++ < 5)
			{
				MODM_LOG_ERROR << "Programming page " << page << " at offset " << offset
							   << " failed with " << err << modm::endl;
			}
		}
		stats.buffer_us += (modm::PreciseClock::now() - start).count();
		stats.buffer_bytes += size;
		stats.buffer_yields += ticks - buffer_ticks;
		verify(slot);
		if (const uint16_t other = random() % slot_count; slots[other].erases) verify(other);
		stats.iterations++;
		return true;
	}
	for (size_t offset{0}; offset < size; offset += WordSize)
	{
		if (not expected(p.seed, p.density, offset / WordSize, word)) continue;
		const uint32_t err = Flash::program(uintptr_t(Flash::getAddr(page) + offset), word);
		if (err and stats.program_errors++ < 5)
		{
			MODM_LOG_ERROR << "Programming page " << page << " at offset " << offset
						   << " failed with " << err << modm::endl;
		}
		stats.words++;
		stats.bytes += WordSize;
	}
	stats.program_us += (modm::PreciseClock::now() - start).count();
	verify(slot);

	// Check that no other page was touched
	if (const uint16_t other = random() % slot_count; slots[other].erases) verify(other);
	stats.iterations++;
	return true;
}

void
report()
{
	MODM_LOG_INFO << stats.iterations << " iterations, " << stats.erases << " erases, "
				  << stats.words << " words, errors: erase " << stats.erase_errors
				  << ", program " << stats.program_errors << ", read " << stats.read_errors
				  << ", verify " << stats.mismatches
				  << ", " << stats.yields << " yields during erases";
	if (stats.erase_us and stats.program_us)
	{
		MODM_LOG_INFO << ", " << (stats.erase_us / stats.erases) << "us/erase, "
					  << (uint64_t(stats.bytes) * 1'000'000 / 1024 / stats.program_us) << "kiB/s program";
	}
	if (stats.buffer_us)
	{
		MODM_LOG_INFO << ", " << (uint64_t(stats.buffer_bytes) * 1'000'000 / 1024 / stats.buffer_us)
					  << "kiB/s program buffer, " << stats.buffer_yields << " yields during buffer programs";
	}
	MODM_LOG_INFO << modm::endl;
}

} // namespace

void
test()
{
	MODM_LOG_INFO << "\n\nFlash stress test for " << FLASH_STRESS_SECONDS << "s\n";
	uint32_t errors{0};

	geometry(errors);
	if (not slot_count)
	{
		MODM_LOG_ERROR << "No free pages!" << modm::endl;
		finishTest(false);
		return;
	}
	if (not Flash::unlock())
	{
		MODM_LOG_ERROR << "Flash unlock failed!" << modm::endl;
		finishTest(false);
		return;
	}

	// A debugger reading the flash during an erase fails, so let RTT attach first
	modm::delay(1s);

	// Units with only erased bytes must stay programmable, the L4 and U5 fail
	// to program a unit again that was programmed with all ones before
	{
		const uint16_t page = ::page(0);
		const uintptr_t addr = uintptr_t(Flash::getAddr(page));
		alignas(4) uint8_t data[2 * WordSize];
		std::memset(data, Flash::ErasedValue, WordSize);
		std::memset(data + WordSize, 0x5a, WordSize);
		uint32_t err = Flash::erase(page);
		err |= Flash::program(addr, data, sizeof(data));
		std::memset(data, 0xa5, WordSize);
		err |= Flash::program(addr, data, WordSize);
		if (err or std::memcmp(Flash::getAddr(page), data, sizeof(data)))
		{
			MODM_LOG_ERROR << "Programming an erased unit again failed with " << err << modm::endl;
			errors++;
		}
		Flash::erase(page);
		slots[0].erases += 2;
	}

	const auto start = modm::Clock::now();
	auto last = start;
	while (modm::Clock::now() - start < std::chrono::seconds(FLASH_STRESS_SECONDS))
	{
		if (not iteration())
		{
			MODM_LOG_INFO << "All pages reached " << FLASH_MAX_ERASES << " erases" << modm::endl;
			break;
		}
		if (modm::Clock::now() - last >= std::chrono::seconds(10))
		{
			last = modm::Clock::now();
			report();
		}
	}
	// Check every page once more
	for (uint16_t slot{0}; slot < slot_count; slot++)
		if (slots[slot].erases) verify(slot);
	Flash::lock();
	report();

	errors += stats.erase_errors + stats.program_errors + stats.read_errors + stats.mismatches;
	finishTest(errors == 0 and stats.iterations);
}

modm::Fiber<1792> fiber_test(test);
modm::Fiber<256> fiber_count([]
{
	while (true)
	{
		ticks = ticks + 1;
		modm::this_fiber::yield();
	}
});

int
main()
{
	Board::initialize();
	modm::fiber::Scheduler::run();
	return 0;
}
