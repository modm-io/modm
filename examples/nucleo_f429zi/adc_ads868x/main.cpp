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
#include <modm/driver/adc/ads868x.hpp>

using namespace Board;

using SpiMaster = SpiMaster1;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
using Cs = GpioA4;
using Rst = GpioF12;

using Ads868x = modm::Ads868x<SpiMaster, Cs, Rst>;

Ads868x adc{};

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MODM_LOG_INFO << "ADS868X Demo on Nucleo-F429ZI\n" << modm::endl;

	SpiMaster::initialize<Board::SystemClock, 10_MHz>();
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();

	Rst::setOutput();
	Cs::setOutput(true);

	MODM_LOG_INFO << "Initializing ADC..." << modm::endl;
	adc.initialize();

	uint32_t counter(0);

	while (true)
	{
		Leds::write(counter % ((1 << (Leds::width+1)) - 1));

		uint16_t result = RF_CALL_BLOCKING(adc.singleConversion());
		MODM_LOG_INFO.printf("ADC Manual: %05u\n", result);

		modm::delay(Button::read() ? 1ms : 500ms);
	}

	return 0;
}
