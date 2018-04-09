// coding: utf-8
/*
 * Copyright (c) 2017, Christopher Durand
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/processing/processing.hpp>
#include <modm/driver/adc/ad7928.hpp>

/**
 * Example to demonstrate an MODM driver for AD7928/AD7918/AD7908 Adc
 *
 * This example uses SpiMaster2 of STM32F746G
 *
 * SCK		PI1
 * MOSI		PB15
 * MISO		PB14
 * CS		PA8
 *
 * A 2.5V reference voltage is applied to Vref.
 */

using Sck = GpioOutputI1;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using Cs = GpioOutputA8;
using SpiMaster = SpiMaster2;

using namespace Board;

using modm::ad7928;

class ThreadOne : public modm::pt::Protothread
{
public:
	static constexpr ad7928::SequenceChannels_t sequence1 =
		ad7928::SequenceChannels::Ch0 |
		ad7928::SequenceChannels::Ch1 |
		ad7928::SequenceChannels::Ch2;

	static constexpr ad7928::SequenceChannels_t sequence2 =
		ad7928::SequenceChannels::Ch0 |
		ad7928::SequenceChannels::Ch4 |
		ad7928::SequenceChannels::Ch5;

	bool
	update()
	{
		PT_BEGIN();

		MODM_LOG_INFO << "Initialize device" << modm::endl;
		PT_CALL(adc.initialize());

		MODM_LOG_INFO << "Test single conversions (Ch 0-2):" << modm::endl;

		// Initiate first conversion, result will be output during the next conversion
		PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch0));

		MODM_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch1)) << modm::endl;
		MODM_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch2)) << modm::endl;

		// Enable auto-shutdown
		adc.setAutoShutdownEnabled(true);

		MODM_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch2)) << modm::endl;

		MODM_LOG_INFO << "Test single conversion with auto-shutdown (Ch 5):" << modm::endl;
		PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch5));
		MODM_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch0)) << modm::endl;

		adc.setAutoShutdownEnabled(false);

		MODM_LOG_INFO << "Test sequence mode" << modm::endl;
		MODM_LOG_INFO << "Program sequence Ch0,Ch1,Ch5, Ch0,Ch4,Ch5" << modm::endl;
		PT_CALL(adc.startSequence(sequence1, sequence2));

		// Run forever
		while (true)
		{
			MODM_LOG_INFO << PT_CALL(adc.nextSequenceConversion()) << modm::endl;

			timeout.restart(500);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::Ad7928<SpiMaster, Cs> adc;
} thread;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Cs::setOutput(modm::Gpio::High);

	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();
	// Initialize the SPI with a 13.5MHz clock (core frequency / 16)
	SpiMaster::initialize<Board::systemClock, 13500000ul>();

	MODM_LOG_INFO << "AD7928/AD7918/AD7908 Example" << modm::endl;

	while (1) {
		thread.update();
	}

	return 0;
}
