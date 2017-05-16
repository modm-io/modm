/*
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2013, 2015-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/architecture/platform.hpp>

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
	GpioD14::connect(Fsmc::D0);
	GpioD15::connect(Fsmc::D1);
	GpioD0::connect(Fsmc::D2);
	GpioD1::connect(Fsmc::D3);
	GpioE7::connect(Fsmc::D4);
	GpioE8::connect(Fsmc::D5);
	GpioE9::connect(Fsmc::D6);
	GpioE10::connect(Fsmc::D7);
	GpioE11::connect(Fsmc::D8);
	GpioE12::connect(Fsmc::D9);
	GpioE13::connect(Fsmc::D10);
	GpioE14::connect(Fsmc::D11);
	GpioE15::connect(Fsmc::D12);
	GpioD8::connect(Fsmc::D13);
	GpioD9::connect(Fsmc::D14);
	GpioD10::connect(Fsmc::D15);

	GpioD4::connect(Fsmc::Noe);
	GpioD5::connect(Fsmc::Nwe);
	GpioB7::connect(Fsmc::Nl); // = Nadv
	GpioD7::connect(Fsmc::Ne1);

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

	for(uint16_t offset = 0; offset <= 0xff; ++offset) {
		// uncomment one
		*(out + offset) = 0xff00 | offset;
		// data[offset] = *(out + offset);
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
