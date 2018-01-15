/*
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2013, 2015-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>

/*
 * This example shows how to use the FSMC in "Muxed Mode"
 * see STM32F4 reference manual p. 1340
 */

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Initialize FSMC
	Fsmc::initialize();
	GpioConnector<Peripheral::Fsmc,
		GpioD14::D0,
		GpioD15::D1,
		GpioD0::D2,
		GpioD1::D3,
		GpioE7::D4,
		GpioE8::D5,
		GpioE9::D6,
		GpioE10::D7,
		GpioE11::D8,
		GpioE12::D9,
		GpioE13::D10,
		GpioE14::D11,
		GpioE15::D12,
		GpioD8::D13,
		GpioD9::D14,
		GpioD10::D15,
		GpioD4::Noe,
		GpioD5::Nwe,
		GpioB7::Nl,
		GpioD7::Ne1>::connect();

	fsmc::NorSram::AsynchronousTiming timing = {
		15, 0, 15,	// Read:  ADDSET, ADDHLD, DATAST
		15, 0, 15,	// Write: ADDSET, ADDHLD, DATAST
		15			// Bus turn around
	};

	fsmc::NorSram::configureAsynchronousRegion(
			fsmc::NorSram::CHIP_SELECT_1,
			fsmc::NorSram::ADDRESS_DATA_MULIPLEX_16BIT,
			fsmc::NorSram::NOR,
			fsmc::NorSram::MODE_A,
			timing,
			fsmc::NorSram::ExtendedMode::Disable);

	fsmc::NorSram::enableRegion(fsmc::NorSram::CHIP_SELECT_1);

	volatile uint16_t * const out = fsmc::NorSram::getRegionPointer<uint16_t>(fsmc::NorSram::CHIP_SELECT_1);

	Board::LedOrange::setOutput(modm::Gpio::High);
	Board::LedGreen::setOutput(modm::Gpio::Low);

	while(!Board::Button::read());

	uint16_t data[0xff];

	for(uint16_t offset = 0; offset < 0xff; ++offset) {
		// uncomment one
		// *(out + offset) = 0xff00 | offset;
		data[offset] = *(out + offset);
	}

	while (1)
	{
		static uint8_t ii = 0;
		++ii;
		Board::LedOrange::toggle();
		Board::LedGreen::toggle();
		modm::delayMilliseconds(data[ii]);
	}

	return 0;
}
