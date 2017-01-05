/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/processing/processing.hpp>
#include <modm/driver/display/max7219matrix.hpp>

/**
 * Example to demonstrate a MODM graphics display made
 * of three 8x8 LED matrices driven by MAX7219 chips.
 *
 * The modules are daisy-chained using SPI.
 *
 * The demo shows a counter counting from 9999 down to 0.
 *
 * The first of three MAX7219 based LED Matrix displays is connected
 * as following:
 *
 * PE1	Data
 * PE3	Cs
 * PE5	Clk
 *
 * GND and +3V3 are connected to the display module.
 * The other modules are daisy-chained.
 *
 */

// Software SPI is simple and fast
typedef GpioOutputE1 Data;
typedef GpioOutputE3 Cs;
typedef GpioOutputE5 Clk;

typedef modm::SoftwareSpiMaster< Clk, Data > Spi;

// The array of 3 modules which all are placed horizontally
modm::Max7219matrix<Spi, Cs, 3, 1> ledMatrixDisplay;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Board::LedOrange::setOutput(modm::Gpio::High);

    Data::setOutput();
    Cs::setOutput();
    Clk::setOutput();

    Spi::initialize< Board::systemClock, 10000000 >();

    ledMatrixDisplay.initialize();

    ledMatrixDisplay.setFont(modm::font::FixedWidth5x8);
	ledMatrixDisplay.setCursor(0, 0);

	modm::ShortPeriodicTimer countdownTimer(100);

    while (1)
    {
		if (countdownTimer.execute())
		{
			for (int16_t sec = 9999; sec >= 0; --sec)
			{
				// Use the LED Matrix as a normal modm buffered graphics display
				ledMatrixDisplay.clear();
				ledMatrixDisplay.printf("%04d", sec);
				ledMatrixDisplay.update();

				Board::LedOrange::toggle();
			}
		}
    }

    return 0;
}
