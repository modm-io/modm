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

/*
 * Stress test of the external RAM of a board, which the board configures before
 * main() and adds to the heap with the MemoryExternal traits. The test uses the
 * whole external heap section, so nothing may be allocated from it before.
 *
 * 1. Data bus: walking ones and zeros in 8, 16 and 32-bit accesses.
 * 2. Address bus: writes to every power-of-two offset must not alias.
 * 3. Byte lanes: 8 and 16-bit writes at every offset and unaligned 32-bit
 *    accesses, which also fault if the memory is mapped as device memory.
 * 4. Full fill: pseudo-random words and their inverse over the whole memory,
 *    also measures the bandwidth.
 * 5. Block copies: memcpy between internal and external RAM and within it.
 * 6. Retention: the data must survive without any accesses, which fails if
 *    the SDRAM refresh is not configured.
 * 7. Random stress: blocks of random size, offset and access width until the
 *    time limit, every block is verified.
 *
 * On a Cortex-M7 the D-cache is cleaned and invalidated before every
 * verification, so that the data is really read from the memory.
 *
 * Lines starting with "R " are results, lines starting with "E " are errors.
 * Any error fails the test.
 */

#include <modm/board.hpp>
#include <modm/architecture/interface/memory.hpp>
#include <modm/architecture/interface/fiber.hpp>
#include <modm/platform/core/heap_table.hpp>
#include <algorithm>
#include <cstring>

#include "../integration_test.hpp"

#ifndef EXTERNAL_MEMORY_STRESS_SECONDS
#define EXTERNAL_MEMORY_STRESS_SECONDS 30
#endif

#define LOG(...) do { MODM_LOG_INFO.printf(__VA_ARGS__); MODM_LOG_INFO.flush(); } while(0)

static uint32_t errors{0};
#define ERROR(...) do { if (errors++ < 20) { LOG("E " __VA_ARGS__); LOG("\n"); } } while(0)

static uint8_t *memory;
static size_t size;

static uint32_t rng_state;
static uint32_t
rng()
{
	rng_state ^= rng_state << 13; rng_state ^= rng_state >> 17; rng_state ^= rng_state << 5;
	return rng_state;
}

static uint32_t
now_ms()
{
	return modm::Clock::now().time_since_epoch().count();
}

/// Writes the D-cache back to memory and discards it, so that reads go to the memory
static void
flush_cache()
{
#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
	SCB_CleanInvalidateDCache();
#endif
	__DSB();
}

// ============================================================================
static void
test_data_bus()
{
	auto *word = reinterpret_cast<volatile uint32_t *>(memory);
	auto *half = reinterpret_cast<volatile uint16_t *>(memory);
	auto *byte = reinterpret_cast<volatile uint8_t *>(memory);
	for (uint32_t bit = 0; bit < 32; bit++)
	{
		for (uint32_t value : {1ul << bit, ~(1ul << bit)})
		{
			*word = value; flush_cache();
			if (*word != value) ERROR("data bus 32-bit %08lx read %08lx", value, *word);
			if (bit < 16) {
				*half = value; flush_cache();
				if (*half != uint16_t(value)) ERROR("data bus 16-bit %04x read %04x", uint16_t(value), *half);
			}
			if (bit < 8) {
				*byte = value; flush_cache();
				if (*byte != uint8_t(value)) ERROR("data bus 8-bit %02x read %02x", uint8_t(value), *byte);
			}
		}
	}
	LOG("R data bus done\n");
}

static void
test_address_bus()
{
	auto *words = reinterpret_cast<volatile uint32_t *>(memory);
	const size_t count = size / 4;
	constexpr uint32_t Pattern{0xAAAAAAAA}, Antipattern{0x55555555};

	for (size_t offset = 1; offset < count; offset <<= 1) words[offset] = Pattern;
	words[0] = Antipattern;
	flush_cache();
	// An address line stuck high aliases every offset to one with that bit set
	for (size_t offset = 1; offset < count; offset <<= 1)
		if (words[offset] != Pattern) ERROR("address bit %u stuck high", __builtin_ctz(offset * 4));

	words[0] = Pattern;
	for (size_t test = 1; test < count; test <<= 1)
	{
		words[test] = Antipattern;
		flush_cache();
		if (words[0] != Pattern) ERROR("address bit %u stuck low", __builtin_ctz(test * 4));
		for (size_t offset = 1; offset < count; offset <<= 1)
			if (offset != test and words[offset] != Pattern)
				ERROR("address bits %u and %u shorted", __builtin_ctz(test * 4), __builtin_ctz(offset * 4));
		words[test] = Pattern;
	}
	LOG("R address bus %u bits done\n", __builtin_ctz(count * 4));
}

static void
test_byte_lanes()
{
	auto *bytes = reinterpret_cast<volatile uint8_t *>(memory);
	auto *words = reinterpret_cast<volatile uint32_t *>(memory);
	for (int ii = 0; ii < 4; ii++) words[ii] = 0;
	for (int ii = 0; ii < 16; ii++) bytes[ii] = 0x10 + ii;
	flush_cache();
	for (int ii = 0; ii < 4; ii++)
	{
		const uint32_t expected = (0x13 + ii * 4) << 24 | (0x12 + ii * 4) << 16 | (0x11 + ii * 4) << 8 | (0x10 + ii * 4);
		if (words[ii] != expected) ERROR("byte lanes word %d %08lx read %08lx", ii, expected, words[ii]);
	}
	auto *halves = reinterpret_cast<volatile uint16_t *>(memory);
	for (int ii = 0; ii < 8; ii++) halves[ii] = 0xA000 + ii;
	flush_cache();
	for (int ii = 0; ii < 8; ii++)
		if (bytes[ii * 2] != ii or bytes[ii * 2 + 1] != 0xA0) ERROR("half word lanes %d", ii);

	// Unaligned word accesses, which fault on device memory
	for (uint32_t offset = 1; offset < 4; offset++)
	{
		uint8_t *address = memory + 32 + offset;
		const uint32_t value = 0x01234567 * offset;
		__UNALIGNED_UINT32_WRITE(address, value);
		flush_cache();
		if (__UNALIGNED_UINT32_READ(address) != value) ERROR("unaligned access at offset %lu", offset);
	}
	LOG("R byte lanes done\n");
}

static void
test_fill()
{
	auto *words = reinterpret_cast<uint32_t *>(memory);
	const size_t count = size / 4;
	for (uint32_t pass = 0; pass < 4; pass++)
	{
		const uint32_t invert = pass & 1 ? 0xffffffff : 0;
		const uint32_t seed = 0x9E3779B9 * (pass / 2 + 1);

		uint32_t start = now_ms();
		rng_state = seed;
		for (size_t ii = 0; ii < count; ii++) words[ii] = rng() ^ invert;
		flush_cache();
		const uint32_t write_ms = now_ms() - start;

		start = now_ms();
		rng_state = seed;
		uint32_t failed{0};
		for (size_t ii = 0; ii < count; ii++)
			if (const uint32_t expected = rng() ^ invert; words[ii] != expected)
				if (failed++ < 4) ERROR("fill pass %lu at %p: %08lx read %08lx", pass, &words[ii], expected, words[ii]);
		const uint32_t read_ms = now_ms() - start;
		if (failed > 4) ERROR("fill pass %lu: %lu more words", pass, failed - 4);
		LOG("R fill pass %lu: write %lu kB/s, verify %lu kB/s\n", pass,
			size / std::max(write_ms, 1ul), size / std::max(read_ms, 1ul));
	}
}

static void
test_copy()
{
	static uint32_t internal[1024];
	rng_state = 0x1234;
	for (auto &word : internal) word = rng();

	// Internal to external at every alignment, and back again
	for (uint32_t offset = 0; offset < 4; offset++)
	{
		std::memcpy(memory + 64 + offset, internal, sizeof(internal));
		flush_cache();
		if (std::memcmp(memory + 64 + offset, internal, sizeof(internal)))
			ERROR("memcpy to external at offset %lu", offset);
	}
	// Within the external memory, from the start to the far end
	const size_t far = size - sizeof(internal) - 64;
	std::memcpy(memory + far, memory + 67, sizeof(internal));
	flush_cache();
	if (std::memcmp(memory + far, internal, sizeof(internal))) ERROR("memcpy within external");
	static uint32_t back[1024];
	std::memcpy(back, memory + far, sizeof(back));
	if (std::memcmp(back, internal, sizeof(back))) ERROR("memcpy from external");
	LOG("R block copies done\n");
}

static void
test_retention()
{
	auto *words = reinterpret_cast<volatile uint32_t *>(memory);
	const size_t count = size / 4;
	rng_state = 0xC0FFEE;
	for (size_t ii = 0; ii < count; ii++) words[ii] = rng();
	flush_cache();
	// No accesses for much longer than the 64ms refresh period of an SDRAM
	modm::this_fiber::sleep_for(std::chrono::seconds(2));
	rng_state = 0xC0FFEE;
	uint32_t failed{0};
	for (size_t ii = 0; ii < count; ii++)
		if (words[ii] != rng()) failed++;
	if (failed) ERROR("retention: %lu words changed after 2s", failed);
	LOG("R retention done\n");
}

static void
test_stress()
{
	const uint32_t end = now_ms() + EXTERNAL_MEMORY_STRESS_SECONDS * 1000;
	uint32_t blocks{0}, bytes{0};
	rng_state = 0xDEADBEEF;
	while (now_ms() < end and errors < 20)
	{
		const uint32_t length = 1 + rng() % 4096;
		const uint32_t offset = rng() % (size - length);
		const uint32_t width = rng() % 4;
		const uint32_t seed = rng() | 1;
		uint8_t *block = memory + offset;

		// Write the block with a random access width, read it back with another one
		uint32_t state = seed;
		const auto next = [&state]() { state ^= state << 13; state ^= state >> 17; state ^= state << 5; return state; };
		switch (width)
		{
			case 0: for (uint32_t ii = 0; ii < length; ii++) block[ii] = next(); break;
			case 1: for (uint32_t ii = 0; ii + 1 < length; ii += 2) { const uint16_t v = next(); std::memcpy(block + ii, &v, 2); }
					if (length & 1) block[length - 1] = next(); break;
			case 2: for (uint32_t ii = 0; ii + 3 < length; ii += 4) { const uint32_t v = next(); std::memcpy(block + ii, &v, 4); }
					for (uint32_t ii = length & ~3u; ii < length; ii++) block[ii] = next(); break;
			default: {
				uint8_t buffer[64];
				for (uint32_t ii = 0; ii < length; ii += sizeof(buffer))
				{
					const uint32_t chunk = std::min<uint32_t>(sizeof(buffer), length - ii);
					for (uint32_t jj = 0; jj < chunk; jj++) buffer[jj] = next();
					std::memcpy(block + ii, buffer, chunk);
				}
			}
		}
		flush_cache();

		// The reference stream is regenerated in the same order as it was written
		state = seed;
		static uint8_t expected[4096];
		switch (width)
		{
			case 1: for (uint32_t ii = 0; ii + 1 < length; ii += 2) { const uint16_t v = next(); std::memcpy(expected + ii, &v, 2); }
					if (length & 1) expected[length - 1] = next(); break;
			case 2: for (uint32_t ii = 0; ii + 3 < length; ii += 4) { const uint32_t v = next(); std::memcpy(expected + ii, &v, 4); }
					for (uint32_t ii = length & ~3u; ii < length; ii++) expected[ii] = next(); break;
			default: for (uint32_t ii = 0; ii < length; ii++) expected[ii] = next();
		}
		if (std::memcmp(block, expected, length))
		{
			uint32_t first = 0;
			while (block[first] == expected[first]) first++;
			ERROR("stress block %lu at %p width %lu: byte %lu is %02x, expected %02x",
				  blocks, block, width, first, block[first], expected[first]);
		}
		blocks++;
		bytes += length;
	}
	LOG("R stress: %lu blocks, %lu kB in %u s\n", blocks, bytes / 1024, EXTERNAL_MEMORY_STRESS_SECONDS);
}

// ============================================================================
int
main()
{
	Board::initialize();
	LOG("External memory test\n");

	const uint8_t *start, *end;
	if (not modm::platform::HeapTable::find_largest(&start, &end, modm::MemoryExternal))
	{
		LOG("E no external memory\n");
		return finishTest(false);
	}
	// Align to 4kB, so that the address bus test toggles single address lines
	memory = reinterpret_cast<uint8_t *>((uintptr_t(start) + 4095) & ~uintptr_t(4095));
	size = end - memory;
	LOG("R memory %p - %p, %u kB\n", memory, end, size / 1024);

	test_data_bus();
	test_address_bus();
	test_byte_lanes();
	test_fill();
	test_copy();
	test_retention();
	test_stress();

	LOG("R %lu errors\n", errors);
	return finishTest(errors == 0);
}
