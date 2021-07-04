/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

#ifdef CFG_TUSB_MCU
#include <modm/debug.hpp>
modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::DiscardIfFull> usb_io_device;
modm::log::Logger modm::log::info(usb_io_device);
#endif
#ifdef TCNT1
constexpr uint32_t SystemCoreClock = F_CPU;
#endif

static void
run_delay_ns(uint32_t ns)
{
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
#ifdef TCNT1
	uint16_t start, stop;
#else
	uint32_t start, stop;
#endif
	{
		modm::atomic::Lock _;
#ifdef TCNT1
		start = TCNT1;
#elif defined DWT
		start = DWT->CYCCNT;
#else
		SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
		SysTick->LOAD = (1ul << 23);
		SysTick->VAL = 0;
		stop = SysTick->VAL;
#endif
		modm::delay_ns(ns);
#ifdef TCNT1
		stop = TCNT1;
#elif defined DWT
		stop = DWT->CYCCNT;
#else
		start = SysTick->VAL;
#endif
	}
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
	const uint32_t cycles = (stop - start) - 4;
	const uint32_t expected = uint64_t(SystemCoreClock) * ns / 1'000'000'000ull;
	const uint32_t real = cycles * 1'000'000'000ull / SystemCoreClock;
	MODM_LOG_INFO.printf("%8lu | %7lu | %7lu | %8lu      %c\n", ns, expected, cycles, real,
	    (cycles < expected*1.2f ? (cycles > expected*0.8f ? ' ' : '<') : '>')) << modm::flush;
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
}

static void
run_delay_us(uint32_t us)
{
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
#ifdef TCNT1
	uint16_t start, stop;
#else
	uint32_t start, stop;
#endif
	{
		modm::atomic::Lock _;
#ifdef TCNT1
		start = TCNT1;
#elif defined DWT
		start = DWT->CYCCNT;
#else
		SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
		SysTick->LOAD = (1ul << 23);
		SysTick->VAL = 0;
		stop = SysTick->VAL;
#endif
		modm::delay_us(us);
#ifdef TCNT1
		stop = TCNT1;
#elif defined DWT
		stop = DWT->CYCCNT;
#else
		start = SysTick->VAL;
#endif
	}
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
	const uint32_t cycles = (stop - start) - 4;
	const uint32_t expected = uint64_t(SystemCoreClock) * us / 1'000'000ull;
	const uint32_t real = cycles * 1'000'000ull / SystemCoreClock;
	MODM_LOG_INFO.printf("%8lu | %8lu | %8lu | %8lu      %c\n", us, expected, cycles, real,
	    (cycles < expected*1.2f ? (cycles > expected*0.8f ? ' ' : '<') : '>')) << modm::flush;
#ifdef CFG_TUSB_MCU
	tud_task();
#endif
}

static void
run_test_ns(bool short_test=false)
{
	MODM_LOG_INFO << "\nmodm::delay_ns for system clock = " << SystemCoreClock << modm::endl;
	MODM_LOG_INFO << "    expected       |      measured\n";
	MODM_LOG_INFO << "      ns |  cycles |  cycles |       ns\n";

	if (short_test)
	{
		run_delay_ns(       100);
		run_delay_ns(     1'000);
		run_delay_ns(    10'000);
		run_delay_ns(   100'000);
		run_delay_ns( 1'000'000);
		run_delay_ns(10'000'000);
	}
	else
	{
		for (uint32_t ii=0; ii <= 10000; ii += 10) run_delay_ns(ii);
		// run_delay_ns(1);
		// run_delay_ns(5);
		// run_delay_ns(10);
		// for (uint32_t ii=  50; ii <   1000; ii +=   50) run_delay_ns(ii);
		// for (uint32_t ii=1000; ii <=  10000; ii +=  100) run_delay_ns(ii);
		run_delay_ns(100'000);
		run_delay_ns(1'000'000);
		run_delay_ns(10'000'000);
	}
}

static void
run_test_us(bool short_test=false)
{
	MODM_LOG_INFO << "\nmodm::delay_us for system clock = " << SystemCoreClock << modm::endl;
	MODM_LOG_INFO << "     expected       |       measured\n";
	MODM_LOG_INFO << "      us |  cycles  |  cycles  |       us\n";

	if (short_test)
	{
		run_delay_us(      1);
		run_delay_us(      5);
		run_delay_us(     10);
		run_delay_us(    100);
		run_delay_us(  1'000);
		run_delay_us( 10'000);
		run_delay_us(100'000);
	}
	else
	{
		for (uint32_t ii=1; ii <= 1000; ii += 1) run_delay_us(ii);
		// for (uint32_t ii=10; ii <= 100; ii += 10) run_delay_us(ii);
		// run_delay_us(1'000);
		run_delay_us( 10'000);
		run_delay_us(100'000);
	}
}

#ifdef TCNT1

int main()
{
	Board::initialize();

	TCCR1A = 0;
	TCCR1B = (1 << CS10);

	run_test_ns();
	run_test_us();

	while(true) {}
	return 0;
}

#elif defined CFG_TUSB_MCU

// SAMD21
int main()
{
	Board::initialize();
	Board::initializeUsbFs();
	tusb_init();

	MODM_LOG_INFO << "Hello World\n";

	static int32_t counter{0};
	while (true)
	{
		tud_task();

		if (counter-- < 0)
		{
			Leds::toggle();
			counter = 1'000'000;
			run_test_ns();
			run_test_us();
		}
	}
	return 0;
}

#else

// STM32 device
struct BootClock
{
	// STM32L4 needs /4, STM32F0 needs /2
	static constexpr float scalar = 1.0 / 1;
	static constexpr uint32_t Usart1 = Rcc::BootFrequency * scalar;
	static constexpr uint32_t Usart2 = Rcc::BootFrequency * scalar;
	static constexpr uint32_t Usart3 = Rcc::BootFrequency * scalar;
	static constexpr uint32_t Usart4 = Rcc::BootFrequency * scalar;
	static constexpr uint32_t Usart5 = Rcc::BootFrequency * scalar;
};
int main()
{
	Board::stlink::Uart::connect<Board::stlink::Tx::Tx>();
	Board::stlink::Uart::initialize<BootClock, 115200_Bd, 5_pct>();

	run_test_ns();
	run_test_us();

	Board::initialize();

	run_test_ns();
	run_test_us();

	while(true) {}
	return 0;
}

#endif
