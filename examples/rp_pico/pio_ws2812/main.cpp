/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include "ws2812.pio.hpp"
using namespace Board;


struct HW
{
	using PIO = modm::platform::Pio0;
	using PIO_SM = PIO::StateMachine<0>;
	using DataGpio = modm::platform::GpioOutput23;
};


int
main()
{


	Board::initialize();

	HW::DataGpio::setOutput(Gpio::OutputType::PushPull, Gpio::SlewRate::Fast);
	HW::DataGpio::setDriveStrength(Gpio::DriveStrength::mA_12);

	auto pio_program_offset = HW::PIO::addProgram(ws2812::program);

	ws2812::init<Board::SystemClock,HW::PIO,HW::PIO_SM,HW::DataGpio>(pio_program_offset);

	constexpr auto delay_val = 5ms;

	while (true)
	{
		uint32_t clr = 0;
		while (clr!=0xff0000) {
			clr = clr + 0x010000;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x010000;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x00ff00) {
			clr = clr + 0x000100;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x000100;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x0000ff) {
			clr = clr + 0x000001;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}
		while (clr!=0x000000) {
			clr = clr - 0x000001;
			HW::PIO_SM::writeBlocking(clr);
			modm::delay(delay_val);
		}

	}

	return 0;
}
