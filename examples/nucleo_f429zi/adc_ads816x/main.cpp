/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/platform/spi/spi_master_1.hpp>
#include <modm/driver/adc/ads816x.hpp>

using namespace Board;

using SpiMaster = SpiMaster1;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
using Cs = GpioA4;
using Rst = GpioF12;

using Ads816x = modm::Ads816x<SpiMaster, Cs>;

Ads816x adc{};

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "ADS816X Demo on Nucleo-F429ZI\n" << modm::endl;

	SpiMaster::initialize<Board::SystemClock, 10_MHz>();
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();

	Rst::setOutput();
	Cs::setOutput(true);

	// Reset ADS816x chip
	Rst::reset();
	modm::delay(1ms);
	Rst::set();
	modm::delay(1ms);

	MODM_LOG_INFO << "Initializing ADC in manual mode..." << modm::endl;
	RF_CALL_BLOCKING(adc.initialize(modm::ads816x::Mode::Manual));

	adc.setTConv(3us);

	uint32_t counter(0);

	while (true)
	{
		Leds::write(counter % ((1 << (Leds::width+1)) - 1));

		MODM_LOG_INFO.printf("\nADC Manual: ");
		for (uint8_t i{0}; i < 8; i++) {
			if (i == 0) {
				// Set channel 0 and 1 as next channels, throw away first two
				//  conversion results
				RF_CALL_BLOCKING(adc.manualModeConversion(0));
				RF_CALL_BLOCKING(adc.manualModeConversion(1));
			}
			uint16_t result = RF_CALL_BLOCKING(adc.manualModeConversion((i + 2) % 8));
			MODM_LOG_INFO.printf(" %05u", result);
		}

		modm::delay(Button::read() ? 1ms : 500ms);
	}

	return 0;
}
