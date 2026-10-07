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

template< class Port >
struct DebugGpioPort : public Port
{
	static void dumpShifts()
	{
		for (int ii=0; ii<11; ii++) {
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
		for (int ii=0; ii<11; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::mask(ii) << modm::endl;
		}
		MODM_LOG_INFO << modm::endl;
		for (int ii=0; ii<11; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::inverted(ii) << modm::endl;
		}
	}
};


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

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

	static_assert(PinGroup::number_of_ports  == 4);
	static_assert(PinGroup2::number_of_ports == 1);
	static_assert(PinGroup3::number_of_ports == 1);
	static_assert(PinGroup4::number_of_ports == 1);

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
	PinGroup::write(0b0000000); check(0b0000000);
	PinGroup::write(0b0000001); check(0b0000001);
	PinGroup::write(0b0000011); check(0b0000011);
	PinGroup::write(0b0000111); check(0b0000111);
	PinGroup::write(0b0001111); check(0b0001111);
	PinGroup::write(0b0011111); check(0b0011111);
	PinGroup::write(0b0111111); check(0b0111111);
	PinGroup::write(0b1111111); check(0b1111111);
	MODM_LOG_INFO << modm::endl;

	PinGroup::reset();
	Pin0::set(); check(0b0000001);
	Pin1::set(); check(0b0000101);
	Pin2::set(); check(0b0001101);
	Pin3::set(); check(0b0011101);
	Pin4::set(); check(0b0111101);
	Pin5::set(); check(0b1111101);

	return finishTest(passed);
}
