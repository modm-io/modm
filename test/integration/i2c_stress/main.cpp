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
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/processing/fiber.hpp>
#include <modm/platform/i2c/bitbang_i2c_master.hpp>

#include "../integration_test.hpp"

#if __has_include(<modm/driver/logic_analyzer.hpp>)
#include <modm/driver/logic_analyzer.hpp>
#endif

/**
 * Stress test of the I2C master with the I2C device on the board.
 *
 * A timer interrupt with a higher priority than the I2C interrupts takes SDA or
 * SCL away from the peripheral at random times and pulls it low for up to 20us.
 * This causes bus errors, lost arbitrations and devices that hold the bus.
 * Meanwhile the test reads an ID register, pings an unused address and aborts
 * running transactions with reset(). The I2cDevice gives up on a transaction
 * after 50ms and resets the master, which also clocks out a stuck device.
 *
 * The test runs first with the I2C peripheral, then with the BitBangI2cMaster
 * on the same pins. Each run passes when no fault occurred, when at least a
 * quarter of the reads returned the right ID and when the bus works again
 * without noise.
 *
 * Boards without an I2C device use I2cMaster1 on the Arduino pins D14 (SDA)
 * and D15 (SCL) with internal pull-ups. Connect D14 to D4 and D15 to D2 for a
 * bit-banged slave in a fiber, which injects NACKs, clock stretching and a held
 * SDA in a deterministic sequence. Without the wires the test only checks that
 * the master survives the noise without a device. Two boards can also test
 * each other, with D14/D15 of one board connected to D4/D2 of the other, so
 * that the slave does not share the CPU with the master it serves.
 *
 * On STM32F4/F7/L1/L4 the DMA logic analyzer records SCL and SDA in a ring buffer
 * and stops when SDA is low for 5 ms, after the first timeout or after the
 * final read:
 *
 *     # SCL=PB6, SDA=PB9 on the DISCO-F407VG
 *     python3 -m modm_tools.logic_analyzer --elf build/scons-release/i2c_stress.elf \
 *         -f modm/openocd.cfg --channels B6,B9 --vcd i2c.vcd --i2c B6,B9
 */
using namespace Board;
using namespace std::chrono_literals;
using namespace modm::platform;

#if __has_include(<modm/driver/touch/ft6x06.hpp>)
namespace dev
{
	using Scl = Board::ft6::Scl;
	using Sda = Board::ft6::Sda;
	using I2c = Board::ft6::I2cMaster;
	constexpr uint8_t Address = Board::ft6::TouchAddress;
	constexpr uint8_t Address2 = Board::ft6::TouchAddress2; // depends on the board revision
	constexpr uint8_t IdRegister[] = {0xA8}; // FocalTech ID
	// The FT6x36 hangs from the noise and needs a reset
	inline void power()
	{
		Board::DisplayReset::setOutput(modm::Gpio::Low);
		modm::delay(10ms);
		Board::DisplayReset::set();
		modm::delay(300ms);
	}
}
#elif defined(STM32F303xC)
namespace dev
{
	using Scl = Board::lsm3::Scl;
	using Sda = Board::lsm3::Sda;
	using I2c = Board::lsm3::I2cMaster;
	constexpr uint8_t Address = 0x1E;    // LSM303DLHC magnetometer
	constexpr uint8_t IdRegister[] = {0x0A}; // IRA_REG_M/IRB_REG_M = "H4"
	inline void power() {}
}
#elif defined(STM32F723xx)
namespace dev
{
	using Scl = GpioA8;
	using Sda = GpioH8;
	using I2c = I2cMaster3;
	constexpr uint8_t Address = 0x38;    // FT6x36 touch controller
	constexpr uint8_t IdRegister[] = {0xA8}; // FocalTech ID
	// The FT6x36 hangs from the noise and needs a reset
	inline void power()
	{
		GpioOutputH9::setOutput(modm::Gpio::Low); // CTP_RST
		modm::delay(10ms);
		GpioOutputH9::set();
		modm::delay(300ms);
	}
}
#elif defined(STM32F746xx)
namespace dev
{
	using Scl = GpioH7;
	using Sda = GpioH8;
	using I2c = I2cMaster3;
	// The FT5336 touch controller at 0x38 returns zeros after power-up
	constexpr uint8_t Address = 0x1A;    // WM8994 audio codec
	constexpr uint8_t IdRegister[] = {0x00, 0x00}; // device ID 0x8994
	inline void power() {}
}
#elif defined(STM32L476xx)
namespace dev
{
	using Scl = GpioB6;
	using Sda = GpioB7;
	using I2c = I2cMaster1;
	constexpr uint8_t Address = 0x4A;    // CS43L22 audio DAC
	constexpr uint8_t IdRegister[] = {0x01}; // chip ID and revision
	inline void power() { GpioOutputE3::setOutput(modm::Gpio::High); modm::delay(10ms); }
}
#elif defined(STM32F429xx)
namespace dev
{
	using Scl = GpioA8;
	using Sda = GpioC9;
	using I2c = I2cMaster3;
	constexpr uint8_t Address = 0x41;    // STMPE811 touch controller
	constexpr uint8_t IdRegister[] = {0x00}; // CHIP_ID, 0x0811
	inline void power() {}
}
#elif defined(STM32F407xx)
namespace dev
{
	using Scl = Board::cs43::Scl;
	using Sda = Board::cs43::Sda;
	using I2c = Board::cs43::I2cMaster;
	constexpr uint8_t Address = 0x4A;    // CS43L22 audio DAC
	constexpr uint8_t IdRegister[] = {0x01}; // chip ID and revision
	inline void power() { Board::cs43::Reset::setOutput(modm::Gpio::High); modm::delay(10ms); }
}
#else
#include "i2c_slave.hpp"
#define I2C_STRESS_SLAVE
namespace dev
{
	using Scl = Board::D15;
	using Sda = Board::D14;
	using I2c = I2cMaster1;
	using Slave = I2cSlave<Board::D2, Board::D4>;
	// The slave misses SCL edges while interrupts run, slow enough to stretch
	constexpr auto Baudrate = 10_kHz;
	constexpr auto BusClear = modm::I2cMaster::ResetDevices::LowSpeed;
	constexpr uint8_t Address = 0x3C;    // bit-banged slave
	constexpr uint8_t IdRegister[] = {0x00}; // 0xC35A
	inline void power() {}
}
#endif
#ifndef I2C_STRESS_SLAVE
namespace dev
{
	constexpr auto Baudrate = 100_kHz;
	constexpr auto BusClear = modm::I2cMaster::ResetDevices::Standard;
}
#endif

// The second run uses the same pins in software
using BitBang = BitBangI2cMaster<dev::Scl, dev::Sda>;

struct Stats
{
	uint32_t loops;
	uint32_t reads_ok;
	uint32_t reads_bad;     // wrong value or failed read
	uint32_t nacks_ok;      // unused address was not acknowledged
	uint32_t nacks_bad;     // unused address was acknowledged
	uint32_t aborts;        // reset() during a running transaction
	uint32_t recover_ok;    // read right after the abort worked
	uint32_t recover_bad;
	uint32_t noise;         // injected glitches
	uint32_t faults;        // assertions, e.g. nvic.undef from a HardFault
	uint32_t fault_context;
	uint32_t timeouts;      // calls that took the whole timeout of 50 ms
	uint32_t errors[8];     // modm::I2cMaster::Error of the failed reads
};
// One for the I2cMaster and one for the BitBangI2cMaster
static volatile Stats results[2];
static volatile Stats *stats{results};

// Keeps the samples before the first call of capture()
static void
capture()
{
#if __has_include(<modm/driver/logic_analyzer.hpp>)
	modm::LogicAnalyzer::stop();
#endif
}

static modm::Abandonment
fault_handler(const modm::AssertionInfo &info)
{
	stats->faults++;
	stats->fault_context = info.context;
	capture();
	printTestResult(false);
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(fault_handler);

static uint32_t rng{0x2545F491};
static uint32_t random() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

// Pulls the pin low for some microseconds, then gives it back to the peripheral
template< class Pin >
static void
glitch(uint32_t us)
{
	auto *port = reinterpret_cast<GPIO_TypeDef *>(GPIOA_BASE + 0x400ul * uint32_t(Pin::port));
	const uint32_t moder = port->MODER;
	const bool high = port->ODR & (1ul << Pin::pin);
	Pin::setOutput(Gpio::OutputType::OpenDrain);
	Pin::reset();
	modm::delay_us(us);
	// only switch the mode back, connect() would also clock out stuck devices
	Pin::set(high);
	port->MODER = moder;
}

static volatile bool noise_enabled{false};
#ifdef STM32G0
MODM_ISR(TIM7_LPTIM2)
#else
MODM_ISR(TIM7)
#endif
{
	Timer7::acknowledgeInterruptFlags(Timer7::InterruptFlag::Update);
	// the same number of glitches per transaction at lower baudrates
	constexpr uint32_t scale = 100'000 / dev::Baudrate;
	Timer7::setPeriod<Board::SystemClock>(std::chrono::microseconds(scale * (50 + random() % 2000)), false);
	if (not noise_enabled) return;
	// A transaction never holds SDA low for this long
	static auto sda_high = modm::PreciseClock::now();
	if (dev::Sda::read()) sda_high = modm::PreciseClock::now();
	else if (modm::PreciseClock::now() - sda_high > 5ms) capture();
	stats->noise++;
	// Take the pin away from the peripheral, pull it low, give it back
	const uint32_t r = random();
	if (r & 1) glitch<dev::Sda>(1 + (r >> 8) % 20);
	else       glitch<dev::Scl>(1 + (r >> 8) % 20);
}

// Calls f() and counts the calls that ran into the timeout of the I2cDevice
template<class F>
static bool
timed(F &&f)
{
	const auto start = modm::Clock::now();
	const bool result = f();
	if (modm::Clock::now() - start >= 50ms) { stats->timeouts++; capture(); }
	return result;
}

template< class Master >
static bool
run(const char *name)
{
	MODM_LOG_INFO << modm::endl << "Testing the " << name << modm::endl;
	dev::power();
#ifdef I2C_STRESS_SLAVE
	constexpr auto pullups = Master::PullUps::Internal;
#else
	constexpr auto pullups = Master::PullUps::External;
#endif
	if constexpr (std::is_same_v<Master, BitBang>)
		Master::template connect<dev::Sda::BitBang, dev::Scl::BitBang>(pullups, dev::BusClear);
	else
		Master::template connect<dev::Sda::Sda, dev::Scl::Scl>(pullups, dev::BusClear);
	Master::template initialize<Board::SystemClock, dev::Baudrate>();

	uint8_t address = dev::Address;
#if __has_include(<modm/driver/touch/ft6x06.hpp>)
	{
		modm::I2cDevice<Master> other{dev::Address2};
		if (other.ping()) address = dev::Address2;
	}
#endif
	modm::I2cDevice<Master> device{address};
	modm::I2cDevice<Master> nobody{0x0B};
	device.setTimeout(50ms);
	nobody.setTimeout(50ms);
	const auto &reg = dev::IdRegister;
	uint8_t id[2]{};

	// Which addresses answer, to verify the pins and the device address
	MODM_LOG_INFO << "Devices:";
	modm::I2cDevice<Master> probe{0};
	for (uint8_t a = 0x08; a < 0x78; a++)
	{
		probe.setAddress(a);
		if (probe.ping()) { MODM_LOG_INFO << " 0x" << modm::hex << a << modm::ascii; }
	}
	MODM_LOG_INFO << modm::endl;

#if defined(STM32F469xx) || defined(STM32F723xx)
	// The FT6x36 misses the first repeated start after reset and then hangs
	const auto warmup = [&]{ (void) device.write(reg, 1); };
#else
	const auto warmup = []{};
#endif
	warmup();
	bool first{false};
	for (uint8_t ii = 0; ii < 3 and not first; ii++)
		first = device.writeRead(reg, sizeof(reg), id, 2);
	MODM_LOG_INFO << "Device 0x" << modm::hex << address << " ID 0x" << id[0] << id[1] << modm::ascii << modm::endl;
	bool has_device{first};
	if (not first)
	{
#ifdef I2C_STRESS_SLAVE
		MODM_LOG_INFO << "No device, only testing the noise and NACKs" << modm::endl;
#else
		return false;
#endif
	}
	const uint32_t expected = uint32_t(id[0] << 8) | id[1];
	const auto matches = [&](const uint8_t *b) { return (uint32_t(b[0] << 8) | b[1]) == expected; };

	noise_enabled = true;
#ifdef I2C_STRESS_SLAVE
	dev::Slave::inject = true;
#endif

	modm::I2cWriteReadTransaction raw{address};
	uint8_t big[16];
	const auto start = modm::Clock::now();
	auto last_print = start;
	while (modm::Clock::now() - start < 20s)
	{
		stats->loops++;
		uint8_t buf[2]{};
		if (not has_device) ;
		else if (timed([&]{ return device.writeRead(reg, sizeof(reg), buf, 2); }) and matches(buf)) stats->reads_ok++;
		else { stats->reads_bad++; stats->errors[uint8_t(Master::getErrorState()) & 7]++; }

		if (timed([&]{ return nobody.ping(); })) stats->nacks_bad++; else stats->nacks_ok++;

		// Abort a running transaction with reset() after a varying delay
		(void) raw.configureWriteRead(reg, sizeof(reg), big, sizeof(big));
		if (Master::start(&raw))
		{
			modm::this_fiber::sleep_for(std::chrono::microseconds(random() % 300));
			Master::reset();
			stats->aborts++;
			if (not has_device) ;
			else if (timed([&]{ return device.writeRead(reg, sizeof(reg), buf, 2); }) and matches(buf)) stats->recover_ok++;
			else stats->recover_bad++;
		}

		if (modm::Clock::now() - last_print >= 2s)
		{
			last_print = modm::Clock::now();
			MODM_LOG_INFO.printf("loops %lu reads %lu/%lu nacks %lu/%lu aborts %lu recover %lu/%lu timeouts %lu noise %lu faults %lu\n",
				stats->loops, stats->reads_ok, stats->reads_bad, stats->nacks_ok, stats->nacks_bad,
				stats->aborts, stats->recover_ok, stats->recover_bad, stats->timeouts, stats->noise, stats->faults);
		}
	}
	noise_enabled = false;
#ifdef I2C_STRESS_SLAVE
	dev::Slave::inject = false;
	const auto &slave = dev::Slave::counters;
	MODM_LOG_INFO.printf("slave starts %lu addressed %lu reads %lu writes %lu faults %lu %lu %lu %lu %lu\n",
		slave.starts, slave.addressed, slave.reads, slave.writes, slave.faults[1],
		slave.faults[2], slave.faults[3], slave.faults[4], slave.faults[5]);
#endif
	// Without noise the bus must work again, after the faults of a device ended
	modm::this_fiber::sleep_for(100ms);
	uint8_t buf[2]{};
	Master::reset();
	dev::power();
	warmup();
	bool final_ok{false};
	// A held SDA may need more than the 9 clocks of one bus clear, and the slave
	// of another board may still inject faults
	if (has_device) for (uint8_t ii = 0; ii < 20 and not final_ok; ii++)
		final_ok = device.writeRead(reg, sizeof(reg), buf, 2) and matches(buf);
	else
	{
		const auto start = modm::Clock::now();
		final_ok = not nobody.ping() and modm::Clock::now() - start < 50ms;
	}
	capture();
	MODM_LOG_INFO.printf("loops %lu reads %lu/%lu nacks %lu/%lu recover %lu/%lu timeouts %lu faults %lu final %d\n",
		stats->loops, stats->reads_ok, stats->reads_bad, stats->nacks_ok, stats->nacks_bad,
		stats->recover_ok, stats->recover_bad, stats->timeouts, stats->faults, final_ok);
	MODM_LOG_INFO.printf("errors reset %lu address %lu data %lu arbitration %lu bus %lu busy %lu unknown %lu\n",
		stats->errors[1], stats->errors[2], stats->errors[3], stats->errors[4], stats->errors[5],
		stats->errors[6], stats->errors[7]);
	const auto &good = has_device ? stats->reads_ok : stats->nacks_ok;
	return final_ok and not stats->faults and good >= stats->loops / 4;
}

static void
test()
{
#if __has_include(<modm/driver/logic_analyzer.hpp>)
	// Keeps the first error of both runs
	modm::LogicAnalyzer::start<Board::SystemClock, dev::Scl, dev::Sda>(500'000, true); // 32 ms
#endif
	// The noise has a higher priority than the I2C interrupts
	Timer7::enable();
	Timer7::setMode(Timer7::Mode::UpCounter);
	Timer7::setPeriod<Board::SystemClock>(500us);
	Timer7::enableInterruptVector(true, 5);
	Timer7::enableInterrupt(Timer7::Interrupt::Update);
	Timer7::start();

	stats = &results[0];
	const bool hardware = run<dev::I2c>("I2cMaster");
	stats = &results[1];
	const bool software = run<BitBang>("BitBangI2cMaster");
	MODM_LOG_INFO.printf("\nI2cMaster %s, BitBangI2cMaster %s\n",
		hardware ? "passed" : "failed", software ? "passed" : "failed");
	printTestResult(hardware and software);
	// the slave keeps serving the master of another board
	while (true) modm::this_fiber::yield();
}

modm::Fiber<4096> test_fiber(test);
#ifdef I2C_STRESS_SLAVE
modm::Fiber<512> slave_fiber(dev::Slave::run);
#endif

int
main()
{
	Board::initialize();
#ifdef I2C_STRESS_SLAVE
	dev::Slave::initialize();
#endif
	modm::fiber::Scheduler::run();
	return 0;
}
