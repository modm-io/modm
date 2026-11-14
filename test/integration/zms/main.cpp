/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// Randomized stress test of ZMS on the internal flash. Four sectors of at least
// 2 kB behind the program image hold 16 IDs, also at the edges of the ID range.
// The test writes, deletes and reads random IDs with values of 1-64 bytes, 1/8
// of them up to 256 bytes and 1/8 ending with 0xFF bytes. It compares ZMS with a
// model in RAM, mounts it again from time to time, like after a reboot, and
// reports the time per operation and the number of erased sectors.
//
// With ZMS_POWER_LOSS_CYCLES defined, the independent watchdog resets the device
// after a random time of up to 2s instead, mostly in the middle of writing or
// erasing the flash. The model survives in the .noinit RAM and after every
// reset each ID must have its old or its new value.

#include <modm/board.hpp>
#include <modm/ext/zephyr-zms/zms.hpp>
#include <modm/platform/flash/flash.hpp>
#include <zephyr/drivers/flash.h>
#include <cstring>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

#ifndef ZMS_STRESS_SECONDS
#define ZMS_STRESS_SECONDS 30
#endif
// Limits the wear of the flash per run
#ifndef ZMS_MAX_ERASES
#define ZMS_MAX_ERASES 40
#endif

extern "C" const uint8_t __rom_end[];

namespace
{

constexpr uint32_t
mix(uint32_t x)
{
	x ^= x >> 16; x *= 0x7feb352d;
	x ^= x >> 15; x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

constexpr size_t Ids{16};
constexpr size_t MaxValue{256};
constexpr int NoId{-1};

/// Fills the value of a seed, which also determines its length
size_t
fill(uint32_t seed, uint8_t *buffer)
{
	const size_t length = 1 + mix(seed) % ((seed & 0x70) ? 64 : MaxValue);
	for (size_t ii{0}; ii < length; ii++) buffer[ii] = mix(seed ^ (ii * 0x01000193));
	// like -1, NaN or erased defaults in a struct
	if (seed % 8 == 0) std::memset(buffer + length / 2, 0xff, length - length / 2);
	return length;
}

/// IDs at the edges of the range, 0xffffffff is reserved by ZMS
constexpr uint32_t
idOf(size_t index)
{ return index < 14 ? index : index == 14 ? 0x7fff'ffff : 0xffff'fffe; }

struct Timing
{
	uint32_t count{}, sum{}, max{};
	void add(uint32_t us) { count++; sum += us; if (us > max) max = us; }
	void print(const char *name)
	{
		MODM_LOG_INFO.printf("%s: %lu x, avg %luus, max %luus\n", name, count,
							 count ? sum / count : 0, max);
	}
};

template< class F >
auto
measure(Timing &timing, F &&f)
{
	const auto start = modm::chrono::micro_clock::now();
	auto result = f();
	timing.add((modm::chrono::micro_clock::now() - start).count());
	return result;
}

/// The model of the storage, which survives a reset
struct State
{
	uint32_t magic;
	uint32_t rng;
	uint32_t seed[Ids];      // 0 = deleted
	int pending;             // index that is written or deleted right now
	uint32_t pending_seed;
	uint32_t cycles, steps, erases, recovered;
};
alignas(32) modm_section(".noinit") State state;
// Changes with every build, so that a new program starts from scratch
constexpr uint32_t Magic = mix(__TIME__[0] | __TIME__[1] << 8 | __TIME__[3] << 16 | __TIME__[4] << 24) ^ __TIME__[7];

uint32_t random(uint32_t max) { return (state.rng = mix(state.rng + 0x9e3779b9)) % max; }

bool failed{false};
#define CHECK(cond, ...) if (not (cond)) { MODM_LOG_ERROR.printf(__VA_ARGS__); failed = true; }

/// The reset can hit any flash operation: the model must be in RAM by then,
/// not only in the write-back data cache of a Cortex-M7
void
persist()
{
#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
	SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(&state), sizeof(state));
#endif
}

// ----------------------------------------------------------------------------
// Counts the erased sectors through a copy of the flash device
const flash_driver_api &flash_api = *modm::zms::device<modm::zms::flash>.api;
flash_driver_api api;
const device dev{&api, nullptr};

int
countErase(const device *d, off_t offset, size_t size)
{
	state.erases++;
	persist();
	return flash_api.erase(d, offset, size);
}

int
persistWrite(const device *d, off_t offset, const void *data, size_t size)
{
	persist();
	return flash_api.write(d, offset, data, size);
}

/// Four sectors of at least 2 kB at the end of the flash, behind the image
bool
setup(zms_fs &fs)
{
	const uint16_t image = Flash::getPage(uintptr_t(__rom_end) - 1);
	uint16_t end{0};
	for (size_t size{0}; size < Flash::Size; size += Flash::getSize(end++)) ;
	const size_t page_size = Flash::getSize(end - 1);
	const uint16_t pages = 4 * ((std::max<size_t>(2048, page_size) + page_size - 1) / page_size);
	if (end <= image + 1 + pages) return false;
	if (not modm::zms::setup(fs, Flash::getOffset(end - pages), 4, 2048)) return false;
	api = flash_api;
	api.erase = countErase;
	api.write = persistWrite;
	fs.flash_device = &dev;
	return true;
}

zms_fs fs;
Timing t_write, t_read, t_delete, t_mount;

void
mount()
{
	const int err = measure(t_mount, [] { return zms_mount(&fs); });
	CHECK(err == 0, "mount failed with %d\n", err);
}

bool
matches(uint32_t seed, const uint8_t *value, ssize_t length)
{
	if (seed == 0) return length == -ENOENT;
	uint8_t expected[MaxValue];
	return length == ssize_t(fill(seed, expected)) and std::memcmp(value, expected, length) == 0;
}

void
verify(size_t index)
{
	uint8_t value[MaxValue]{};
	const ssize_t length = measure(t_read, [&] { return zms_read(&fs, idOf(index), value, MaxValue); });
	if (int(index) == state.pending)
	{
		// Interrupted by a reset: the old or the new value
		const bool is_new = matches(state.pending_seed, value, length);
		CHECK(is_new or matches(state.seed[index], value, length),
			  "id %lx: interrupted, but neither old nor new value (%d)\n", idOf(index), int(length));
		if (is_new) state.seed[index] = state.pending_seed;
		state.pending = NoId;
		state.recovered++;
		return;
	}
	CHECK(matches(state.seed[index], value, length),
		  "id %lx: read %d, wrong length or data\n", idOf(index), int(length));
}

void
verifyAll()
{
	for (size_t index{0}; index < Ids; index++) verify(index);
}

void
step()
{
	const size_t index = random(Ids);
	const uint32_t op = random(10);
	if (op < 6)
	{
		const uint32_t seed = random(0xffff'fffe) + 1;
		uint8_t value[MaxValue];
		const size_t length = fill(seed, value);
		state.pending_seed = seed;
		state.pending = index;
		const ssize_t err = measure(t_write, [&] { return zms_write(&fs, idOf(index), value, length); });
		// zero if the value is already stored
		CHECK(err == ssize_t(length) or err == 0, "id %lx: write failed with %d\n", idOf(index), int(err));
		state.seed[index] = seed;
		state.pending = NoId;
	}
	else if (op < 7)
	{
		state.pending_seed = 0;
		state.pending = index;
		const int err = measure(t_delete, [&] { return zms_delete(&fs, idOf(index)); });
		CHECK(err == 0, "id %lx: delete failed with %d\n", idOf(index), err);
		state.seed[index] = 0;
		state.pending = NoId;
	}
	verify(index);
	state.steps++;
}

void
report()
{
	MODM_LOG_INFO.printf("%lu steps, %lu erased sectors, %lu cycles, recovered %lu values\n",
						 state.steps, state.erases, state.cycles, state.recovered);
	t_write.print("write");
	t_read.print("read");
	t_delete.print("delete");
	t_mount.print("mount");
}

} // namespace

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	// A failed run must not restart and wear the flash further
	if (state.magic == ~Magic) return finishTest(false);
	const bool fresh = state.magic != Magic;
	if (fresh) state = State{Magic, 0x12345678, {}, NoId, 0, 0, 0, 0, 0};
	else state.cycles++;
	if (fresh) modm::delay(1s); // let RTT attach before the flash stalls the bus

	if (not setup(fs))
	{
		MODM_LOG_ERROR << "No free pages!" << modm::endl;
		return finishTest(false);
	}
	if (fresh)
	{
		MODM_LOG_INFO << "\n\nZMS stress test, write block " << Flash::WriteSize << " bytes\n"
					  << "4 sectors at " << (Flash::OriginAddr + fs.offset) << " with "
					  << fs.sector_size << " bytes" << modm::endl;
		// Other tests may have left data in these pages
		for (uint32_t sector{0}; sector < fs.sector_count; sector++)
			api.erase(&dev, fs.offset + sector * fs.sector_size, fs.sector_size);
	}
	mount();
	if (not failed) verifyAll();
	if (failed) { state.magic = ~Magic; return finishTest(false); }

#ifdef ZMS_POWER_LOSS_CYCLES
	if (fresh)
		MODM_LOG_INFO << "Power loss test with " << ZMS_POWER_LOSS_CYCLES << " resets" << modm::endl;
	else if (state.cycles % 10 == 0)
		MODM_LOG_INFO << "cycle " << state.cycles << ", " << state.steps << " steps, "
					  << state.erases << " erases" << modm::endl;
	if (state.cycles < ZMS_POWER_LOSS_CYCLES and state.erases < ZMS_MAX_ERASES)
	{
		// The watchdog resets after 50-300ms, the LSI clocks it with ~32kHz / 32.
		// It keeps running after the reset, the startup must be shorter.
		Iwdg::enable();
		Iwdg::initialize(Iwdg::Prescaler::Div32, 50 + random(250));
		Iwdg::trigger();
		while (not failed) step();
		state.magic = ~Magic;
		printTestResult(false);
		while (true) Iwdg::trigger();
	}
#else
	const auto start = modm::Clock::now();
	uint32_t remounts{0};
	while (not failed and modm::Clock::now() - start < std::chrono::seconds(ZMS_STRESS_SECONDS)
		   and state.erases < ZMS_MAX_ERASES)
	{
		step();
		if (random(128) == 0) { mount(); verifyAll(); remounts++; }
	}
	MODM_LOG_INFO << remounts << " remounts" << modm::endl;
	mount();
	verifyAll();
#endif
	report();
	state.magic = 0;
	return finishTest(not failed);
}
