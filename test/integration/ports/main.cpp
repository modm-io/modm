/*
 * Copyright (c) 2018, Niklas Hauser
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

/**
 * Tests that software and hardware GPIO ports write and read back the right
 * bits, also with unused and inverted pins and pins spread over several ports.
 */
#ifdef MODM_CPU_AVR
// The four LEDs of the AL-AVREB-CAN are all on port F
constexpr int Ports = 7;
using Pin0 = Led0; // GpioF0
using Pin1 = Led1; // GpioF1
using Pin2 = Led2; // GpioF2
using Pin3 = Led3; // GpioF3

using PinGroup = SoftwareGpioPort< Pin3, Pin2, Pin1, GpioUnused, Pin0 >;
using PinGroup2 = SoftwareGpioPort< GpioInverted<GpioOutputF0>, GpioInverted<GpioOutputF1>, GpioInverted<GpioOutputF2>, GpioInverted<GpioOutputF3> >;
using PinGroup3 = GpioPort< GpioF0, +4 >;
using PinGroup4 = GpioPort< GpioInverted<GpioF3>, -4 >;
static_assert(PinGroup::number_of_ports == 1);

static void
setPins(auto check)
{
	Pin0::set(); check(0b00001);
	Pin1::set(); check(0b00101);
	Pin2::set(); check(0b01101);
	Pin3::set(); check(0b11101);
}
#else
// The six LEDs of the STM32F469I-DISCO are spread over four ports
constexpr int Ports = 11;
using Pin0 = LedGreen;  // GpioG6;
using Pin1 = LedOrange; // GpioD4;
using Pin2 = LedRed;    // GpioD5;
using Pin3 = LedBlue;   // GpioK3;
using Pin4 = LedUsb;    // GpioB7;
using Pin5 = LedD13;    // GpioD3;

using PinGroup = SoftwareGpioPort< Pin5, Pin4, Pin3, Pin2, Pin1, GpioUnused, Pin0 >;
using PinGroup2 = SoftwareGpioPort< GpioB7, GpioB6, GpioB5, GpioB4, GpioB3, GpioB2, GpioB1, GpioB0 >;
using PinGroup3 = GpioPort< GpioB0, +8 >;
using PinGroup4 = GpioPort< GpioInverted<GpioB7>, -8 >;
static_assert(PinGroup::number_of_ports == 4);

static void
setPins(auto check)
{
	Pin0::set(); check(0b0000001);
	Pin1::set(); check(0b0000101);
	Pin2::set(); check(0b0001101);
	Pin3::set(); check(0b0011101);
	Pin4::set(); check(0b0111101);
	Pin5::set(); check(0b1111101);
}
#endif
static_assert(PinGroup2::number_of_ports == 1);
static_assert(PinGroup3::number_of_ports == 1);
static_assert(PinGroup4::number_of_ports == 1);

template< class Port >
struct DebugGpioPort : public Port
{
	static void dumpShifts()
	{
		for (int ii=0; ii<Ports; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " {";
			for (int jj=0; jj<Port::width; jj++) {
				const auto p = Port::shift_masks[ii][jj];
				MODM_LOG_INFO << " ";
				if (p >= 0) { MODM_LOG_INFO << p; }
				else { MODM_LOG_INFO << " "; }
				MODM_LOG_INFO << " ";
				if (jj != Port::width - 1) { MODM_LOG_INFO << ","; }
			}
			MODM_LOG_INFO << "}" << modm::endl;
		}
	}

	static void dumpMasks()
	{
		for (int ii=0; ii<Ports; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::mask(ii) << modm::endl;
		}
		MODM_LOG_INFO << modm::endl;
		for (int ii=0; ii<Ports; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::inverted(ii) << modm::endl;
		}
	}
};

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	DebugGpioPort<PinGroup>::dumpMasks();  MODM_LOG_INFO << modm::endl;
	DebugGpioPort<PinGroup2>::dumpMasks(); MODM_LOG_INFO << modm::endl;
	DebugGpioPort<PinGroup3>::dumpMasks(); MODM_LOG_INFO << modm::endl;
	DebugGpioPort<PinGroup4>::dumpMasks(); MODM_LOG_INFO << modm::endl;

	DebugGpioPort<PinGroup>::dumpShifts();  MODM_LOG_INFO << modm::endl;
	DebugGpioPort<PinGroup2>::dumpShifts(); MODM_LOG_INFO << modm::endl;

	PinGroup2::setInput(); MODM_LOG_INFO << modm::bin << PinGroup2::read() << modm::endl;
	PinGroup3::setInput(); MODM_LOG_INFO << modm::bin << PinGroup3::read() << modm::endl;
	PinGroup4::setInput(); MODM_LOG_INFO << modm::bin << PinGroup4::read() << modm::endl;
	MODM_LOG_INFO << modm::endl;

	PinGroup::setOutput(modm::Gpio::High); modm::delay(1s);

	bool passed = true;
	const auto check = [&passed](uint16_t value)
	{
		modm::delay(10ms);
		const uint16_t read = PinGroup::read();
		MODM_LOG_INFO << modm::bin << read << modm::endl;
		// the unused pin always reads low
		passed &= (read == (value & ~0b10));
	};

	// we must read back what we wrote
	for (uint8_t bits = 0; bits <= PinGroup::width; bits++)
	{
		const uint16_t value = (1u << bits) - 1;
		PinGroup::write(value); check(value);
	}
	MODM_LOG_INFO << modm::endl;

	PinGroup::reset();
	setPins(check);

	return finishTest(passed);
}
