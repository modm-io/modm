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

using namespace Board;

template< class Port >
struct DebugGpioPort : public Port
{
	static void dumpShifts()
	{
		for (int ii=0; ii<7; ii++) {
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
		for (int ii=0; ii<7; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::mask(ii) << modm::endl;
		}
		MODM_LOG_INFO << modm::endl;
		for (int ii=0; ii<7; ii++) {
			MODM_LOG_INFO << char(ii+'A') << " " << modm::bin << Port::inverted(ii) << modm::endl;
		}
	}
};

// ----------------------------------------------------------------------------
int main()
{
	Board::initialize();

	using Pin0 = Led0; // GpioF0
	using Pin1 = Led1; // GpioF1
	using Pin2 = Led2; // GpioF2
	using Pin3 = Led3; // GpioF3

	using PinGroup = SoftwareGpioPort< Pin3, Pin2, Pin1, GpioUnused, Pin0 >;
	using PinGroup2 = SoftwareGpioPort< GpioInverted<GpioOutputF0>, GpioInverted<GpioOutputF1>, GpioInverted<GpioOutputF2>, GpioInverted<GpioOutputF3> >;
	using PinGroup3 = GpioPort< GpioF0, +4 >;
	using PinGroup4 = GpioPort< GpioInverted<GpioF3>, -4 >;

	static_assert(PinGroup::number_of_ports  == 1);
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

	const auto fn_report = []
	{
		MODM_LOG_INFO << modm::bin << PinGroup::read() << modm::endl; modm::delay(200ms);
	};

	while (true)
	{
		PinGroup::write(0b00000); fn_report();
		PinGroup::write(0b00001); fn_report();
		PinGroup::write(0b00011); fn_report();
		PinGroup::write(0b00111); fn_report();
		PinGroup::write(0b01111); fn_report();
		PinGroup::write(0b11111); fn_report();
		MODM_LOG_INFO << modm::endl;

		PinGroup::reset();
		Pin0::set(); fn_report();
		Pin1::set(); fn_report();
		Pin2::set(); fn_report();
		Pin3::set(); fn_report();
		MODM_LOG_INFO << modm::endl;

		// while (true);
	}
}
