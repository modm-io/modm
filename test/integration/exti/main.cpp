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

#include "../integration_test.hpp"

using namespace Board;
using namespace std::chrono_literals;

#ifdef STM32H7R3xx
// The WeAct H7R3Z8 has no Arduino header: use pins on ports A-E that are not
// connected to the XSPI flash, USB, crystals, SWD, LED or button.
// Build it with `modm:board:weact-h7r3z8:logger` set to RTT.
using A0 = GpioA0;  using A1 = GpioB1;  using A2 = GpioC2;  using A3 = GpioD3;
using D2 = GpioE4;  using D3 = GpioD5;  using D4 = GpioE6;  using D5 = GpioA7;
using D6 = GpioB8;  using D7 = GpioC9;  using D8 = GpioD10; using D9 = GpioD11;
using D10 = GpioC12; using D12 = GpioD13; using D14 = GpioD14; using D15 = GpioB15;
#endif

/**
 * Toggles the free pins of the Arduino header as outputs and checks that the
 * EXTI handler sees the same edges on the same line. The input stage of a GPIO
 * stays enabled in output mode, so no external wiring is required.
 */
static bool passed{true};
static volatile uint32_t edges{};
static volatile uint8_t last_line{0xff};
constexpr uint32_t Toggles{10};

template< class Pin >
static uint32_t
toggle()
{
	edges = 0;
	for (uint32_t ii = 0; ii < Toggles; ii++)
	{
		Pin::set();   modm::delay(10us);
		Pin::reset(); modm::delay(10us);
	}
	return edges;
}

template< class Pin >
static void
test_pin(const char* name)
{
	Pin::setOutput(modm::Gpio::Low);
	last_line = 0xff;
	Exti::connect<Pin>(Exti::Trigger::BothEdges, [](uint8_t line) { edges = edges + 1; last_line = line; });
	const uint32_t both = toggle<Pin>();
	const uint8_t line = last_line;
	// Select another port at runtime, then this one again. Switching to a pin
	// with another level is an edge itself (PA13-15 are pulled), so wait for it.
	// PA13/PA14 toggle while the debugger reads RTT, so use port B/C for them.
	constexpr bool swd = Pin::port == Gpio::Port::A or Pin::pin == 13 or Pin::pin == 14;
	constexpr auto other_port = not swd ? Gpio::Port::A :
		(Pin::port == Gpio::Port::B ? Gpio::Port::C : Gpio::Port::B);
	Exti::setTriggerSource(Pin::pin, other_port);
	modm::delay(10us);
	const uint32_t other = toggle<Pin>();
	Exti::setTriggerSource(Pin::pin, Pin::port);
	modm::delay(10us);
	const uint32_t runtime = toggle<Pin>();
	Exti::setTriggers<Pin>(Exti::Trigger::RisingEdge);
	const uint32_t rising = toggle<Pin>();
	Exti::setTriggers<Pin>(Exti::Trigger::FallingEdge);
	const uint32_t falling = toggle<Pin>();
	Exti::disconnect<Pin>();
	const uint32_t none = toggle<Pin>();
	Pin::setInput();

	const bool ok = both == 2*Toggles and other == 0 and runtime == 2*Toggles and
					rising == Toggles and falling == Toggles and none == 0 and line == Pin::pin;
	MODM_LOG_INFO.printf("%-3s P%c%-2u  both=%2lu other=%2lu runtime=%2lu rising=%2lu falling=%2lu "
						 "none=%2lu line=%3u  %s\n", name, char('A' + uint8_t(Pin::port)), Pin::pin,
						 both, other, runtime, rising, falling, none, line, ok ? "ok" : "FAIL") << modm::flush;
	if (not ok) passed = false;
}

static volatile uint32_t line_edges[16];

/**
 * Connects two pins of the same EXTI vector and toggles them together.
 * After disconnecting the first pin, the vector must stay enabled for the second.
 * Returns false if the pins do not share a vector.
 */
template< class Pin1, class Pin2 >
static bool
test_shared(const char* name1, const char* name2)
{
	constexpr auto vector = Exti::getVectorForLine(Pin1::pin);
	if constexpr (Pin1::pin == Pin2::pin or vector != Exti::getVectorForLine(Pin2::pin)) return false;
	else
	{
		const auto toggle_both = []()
		{
			for (auto& e : line_edges) e = 0;
			for (uint32_t ii = 0; ii < Toggles; ii++)
			{
				Pin1::set(); Pin2::set();     modm::delay(10us);
				Pin1::reset(); Pin2::reset(); modm::delay(10us);
			}
		};
		Pin1::setOutput(modm::Gpio::Low);
		Pin2::setOutput(modm::Gpio::Low);
		const auto handler = [](uint8_t line) { line_edges[line] = line_edges[line] + 1; };
		Exti::connect<Pin1>(Exti::Trigger::BothEdges, handler);
		Exti::connect<Pin2>(Exti::Trigger::BothEdges, handler);
		toggle_both();
		const uint32_t both1 = line_edges[Pin1::pin], both2 = line_edges[Pin2::pin];
		Exti::disconnect<Pin1>();
		const bool enabled = NVIC_GetEnableIRQ(IRQn_Type(vector));
		toggle_both();
		const uint32_t single1 = line_edges[Pin1::pin], single2 = line_edges[Pin2::pin];
		Exti::disconnect<Pin2>();
		const bool disabled = not NVIC_GetEnableIRQ(IRQn_Type(vector));
		Pin1::setInput();
		Pin2::setInput();

		const bool ok = both1 == 2*Toggles and both2 == 2*Toggles and single1 == 0 and
						single2 == 2*Toggles and enabled and disabled;
		MODM_LOG_INFO.printf("%s+%s on IRQ %d: both=%2lu+%2lu single=%2lu+%2lu enabled=%u disabled=%u  %s\n",
							 name1, name2, int(vector), both1, both2, single1, single2,
							 enabled, disabled, ok ? "ok" : "FAIL") << modm::flush;
		if (not ok) passed = false;
		return true;
	}
}

// The CMSIS header tells us which registers exist for the lines 32 and above
template< class T > concept HasMasks2 = requires(T t) { t.IMR2; };
template< class T > concept HasLines2 = requires(T t) { t.RTSR2; };

/**
 * Enables all lines 32+ and triggers them in software. Their vectors stay
 * disabled. Checks that the 64-bit API reaches the second register bank.
 */
static void
test_lines_32()
{
	constexpr bool masks = HasMasks2<EXTI_TypeDef>;
	constexpr bool lines = HasLines2<EXTI_TypeDef>;
	const Exti::MaskType upper = ~Exti::MaskType(0xffff'ffff);

	Exti::enableInterrupts(upper);
	const auto enabled = Exti::getInterruptEnabled() & upper;
	Exti::setTrigger(upper, Exti::Trigger::RisingEdge);
	Exti::setFlags(upper);
	modm::delay(1us);
	const auto flags = Exti::getFlags() & upper;
	Exti::acknowledgeFlags(upper);
	const auto cleared = Exti::getFlags() & upper;
	Exti::setTrigger(upper, Exti::Trigger::NoEdges);
	Exti::disableInterrupt(upper);

	const bool ok = (sizeof(Exti::MaskType) == 8) == masks and
					bool(enabled) == masks and bool(flags) == lines and not cleared;
	MODM_LOG_INFO.printf("Lines 32+: IMR2=%u RTSR2=%u  enabled=%08lx flags=%08lx cleared=%08lx  %s\n",
						 masks, lines, uint32_t(uint64_t(enabled) >> 32), uint32_t(uint64_t(flags) >> 32),
						 uint32_t(uint64_t(cleared) >> 32), ok ? "ok" : "FAIL") << modm::flush;
	if (not ok) passed = false;
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "\nEXTI loopback test on the Arduino header pins\n" << modm::flush;

	// A4/A5 may be I2C, D0/D1 the UART, D13 an LED and D11 the Ethernet RMII_CRS_DV.
	test_pin<A0>("A0");
	test_pin<A1>("A1");
	test_pin<A2>("A2");
	test_pin<A3>("A3");
	test_pin<D2>("D2");
	test_pin<D3>("D3");
	test_pin<D4>("D4");
	test_pin<D5>("D5");
	test_pin<D6>("D6");
	test_pin<D7>("D7");
	test_pin<D8>("D8");
	test_pin<D9>("D9");
	test_pin<D10>("D10");
	test_pin<D12>("D12");
	test_pin<D14>("D14");
	test_pin<D15>("D15");

	// Use the first pair of pins that shares an EXTI vector
	if (not (test_shared<D4, D5>("D4", "D5") or test_shared<D2, D6>("D2", "D6") or
			 test_shared<D3, D9>("D3", "D9") or test_shared<D7, D8>("D7", "D8") or
			 test_shared<A0, A1>("A0", "A1")))
		MODM_LOG_INFO << "No pins share an EXTI vector" << modm::endl;

	test_lines_32();

	return finishTest(passed);
}
