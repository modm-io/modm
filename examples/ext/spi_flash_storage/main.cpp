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

// Storage on the W25QxxJV SPI flash, which can be soldered onto the back of the
// WeAct MiniF411 or MiniF401 (Black Pill): reads the JEDEC ID, programs and
// reads back one sector, then stresses ZMS for 30 seconds and compares it with a
// model in RAM.
//
// The board has no debugger, so upload it with the DFU bootloader (hold BOOT0
// and press NRST) and `scons program-dfu`. The log is on the USB serial port,
// the test starts once the port is opened. Sending a 'B' restarts the board
// into the DFU bootloader for the next upload.
#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>
#include <modm/ext/zephyr-zms/zms.hpp>
#include <modm/processing.hpp>
#include <tusb.h>
#include <algorithm>
#include <cstring>

using namespace Board;
using namespace std::chrono_literals;

// ----------------------------------------------------------------------------
// Logger over USB CDC, waits for the host instead of dropping output
struct UsbLog : modm::IODevice
{
	void write(char c) override
	{ while (not UsbUart0::write(uint8_t(c))) modm::this_fiber::yield(); }
	void flush() override {}
	bool read(char &) override { return false; }
} usbLog;
modm::log::Logger modm::log::debug(usbLog);
modm::log::Logger modm::log::info(usbLog);
modm::log::Logger modm::log::warning(usbLog);
modm::log::Logger modm::log::error(usbLog);

constexpr uint32_t BootMagic{0xb007'10ad};
modm_section(".noinit") uint32_t bootMagic;

void
jumpToBootloaderIfRequested()
{
	if (bootMagic != BootMagic) return;
	bootMagic = 0;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0; // system flash at 0
	__set_MSP(*reinterpret_cast<volatile uint32_t *>(0x1FFF'0000));
	reinterpret_cast<void (*)()>(*reinterpret_cast<volatile uint32_t *>(0x1FFF'0004))();
}

modm::Fiber<2048> usbFiber([]
{
	while (true)
	{
		tud_task();
		uint8_t c;
		if (UsbUart0::read(c) and c == 'B') { bootMagic = BootMagic; NVIC_SystemReset(); }
		modm::this_fiber::yield();
	}
});

// ----------------------------------------------------------------------------
// Uses the first 2 MB, so that it works with every W25Q16JV or larger:
// ZMS at 1 MB, the read back test in the last sector
constexpr uint32_t Size = 2 * 1024 * 1024;
modm::BdSpiFlash<SpiMaster1, flash::Cs, Size> storage;

constexpr uint32_t
mix(uint32_t x)
{
	x ^= x >> 16; x *= 0x7feb352d;
	x ^= x >> 15; x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}
uint32_t rng{0x12345678};
uint32_t random(uint32_t max) { return (rng = mix(rng + 0x9e3779b9)) % max; }

constexpr size_t MaxValue{256};
size_t
fill(uint32_t seed, uint8_t *buffer)
{
	const size_t length = 1 + mix(seed) % ((seed & 0x70) ? 64 : MaxValue);
	for (size_t ii{0}; ii < length; ii++) buffer[ii] = mix(seed ^ (ii * 0x01000193));
	if (seed % 8 == 0) std::memset(buffer + length / 2, 0xff, length - length / 2);
	return length;
}

bool failed{false};
#define CHECK(cond, ...) if (not (cond)) { MODM_LOG_ERROR.printf(__VA_ARGS__); failed = true; }

// ----------------------------------------------------------------------------
constexpr size_t Ids{16};
constexpr uint32_t ZmsOffset{0x10'0000};

/// IDs at the edges of the range, 0xffffffff is reserved by ZMS
constexpr uint32_t
idOf(size_t index)
{ return index < 14 ? index : index == 14 ? 0x7fff'ffff : 0xffff'fffe; }

bool
matches(uint32_t seed, const uint8_t *value, ssize_t length)
{
	if (seed == 0) return length == -ENOENT;
	uint8_t expected[MaxValue];
	return length == ssize_t(fill(seed, expected)) and std::memcmp(value, expected, length) == 0;
}

void
testZms(std::chrono::seconds duration)
{
	zms_fs fs;
	uint32_t seeds[Ids]{};
	CHECK(storage.erase(ZmsOffset, 8 * 4096), "erase failed\n");
	CHECK(modm::zms::setup<storage>(fs, ZmsOffset, 8), "setup failed\n");
	CHECK(zms_mount(&fs) == 0, "mount failed\n");
	uint32_t steps{}, remounts{};
	const auto start = modm::Clock::now();
	while (not failed and modm::Clock::now() - start < duration)
	{
		const size_t index = random(Ids);
		const uint32_t id = idOf(index);
		if (const uint32_t op = random(10); op < 6)
		{
			const uint32_t seed = random(0xffff'fffe) + 1;
			uint8_t value[MaxValue];
			const size_t length = fill(seed, value);
			const ssize_t err = zms_write(&fs, id, value, length);
			CHECK(err == ssize_t(length) or err == 0, "zms write %lx: %d\n", id, int(err));
			seeds[index] = seed;
		}
		else if (op < 7)
		{
			CHECK(zms_delete(&fs, id) == 0, "zms delete %lx\n", id);
			seeds[index] = 0;
		}
		if (random(128) == 0) { CHECK(zms_mount(&fs) == 0, "remount failed\n"); remounts++; }
		// the changed ID, and all of them from time to time
		for (size_t ii = (steps % 32 ? index : 0); ii < (steps % 32 ? index + 1 : Ids); ii++)
		{
			const uint32_t rid = idOf(ii);
			uint8_t value[MaxValue];
			const ssize_t length = zms_read(&fs, rid, value, MaxValue);
			CHECK(matches(seeds[ii], value, length), "zms id %lx: read %d wrong\n", rid, int(length));
		}
		steps++;
	}
	MODM_LOG_INFO.printf("ZMS: %lu steps, %lu remounts, free %d bytes\n",
						 steps, remounts, int(zms_calc_free_space(&fs)));
}

// ----------------------------------------------------------------------------
modm::Fiber<8192> testFiber([]
{
	while (not tud_cdc_connected()) modm::this_fiber::sleep_for(10ms);
	modm::this_fiber::sleep_for(200ms);
	MODM_LOG_INFO << "\nSPI flash storage" << modm::endl;
	SpiMaster1::connect<flash::Sck::Sck, flash::Mosi::Mosi, flash::Miso::Miso>();
	SpiMaster1::initialize<SystemClock, SystemClock::Spi1 / 8>(); // 12 or 10.5 MHz
	CHECK(storage.initialize(), "initialize failed\n");
	const auto id = storage.readId();
	MODM_LOG_INFO.printf("JEDEC ID: %02x %02x %02x, %lu kB\n", id.manufacturerId, id.deviceType, id.deviceId,
						 (1ul << id.deviceId) / 1024);
	CHECK(id.manufacturerId != 0 and id.manufacturerId != 0xff, "no flash found\n");
	CHECK(id.deviceId >= 21 and id.deviceId < 25, "flash smaller than 2 MB\n");
	if (not failed)
	{
		// Programs across the 256 byte pages, at odd offsets and lengths
		static uint8_t data[4096], readback[4096];
		for (size_t ii{0}; ii < sizeof(data); ii++) data[ii] = mix(ii);
		const auto start = modm::Clock::now();
		CHECK(storage.erase(Size - 4096, 4096), "erase failed\n");
		CHECK(storage.read(readback, Size - 4096, 4096), "read failed\n");
		CHECK(std::all_of(readback, readback + 4096, [](uint8_t b) { return b == 0xff; }), "not erased\n");
		CHECK(storage.program(data, Size - 4096, 100) and storage.program(data + 100, Size - 4096 + 100, 1000) and
			  storage.program(data + 1100, Size - 4096 + 1100, 2996), "program failed\n");
		CHECK(storage.read(readback, Size - 4096, 4096) and std::equal(data, data + 4096, readback), "wrong data\n");
		MODM_LOG_INFO << "Program and read back 4 kB: " << (modm::Clock::now() - start) << modm::endl;
	}
	if (not failed) testZms(30s);
	MODM_LOG_INFO << modm::endl << (failed ? "FAILED" : "PASSED") << modm::endl;
	while (true) { Led::toggle(); modm::this_fiber::sleep_for(500ms); }
});

int
main()
{
	jumpToBootloaderIfRequested();
	Board::initialize();
	Board::initializeUsb();
	tusb_init();
	modm::fiber::Scheduler::run();
}
