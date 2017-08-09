// coding: utf-8
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/adc/ad7928.hpp>

/**
 * Example to demonstrate an XPCC driver for AD7928/AD7918/AD7908 Adc
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

using xpcc::ad7928;

class ThreadOne : public xpcc::pt::Protothread
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

		XPCC_LOG_INFO << "Initialize device" << xpcc::endl;
		PT_CALL(adc.initialize());

		XPCC_LOG_INFO << "Test single conversions (Ch 0-2):" << xpcc::endl;

		// Initiate first conversion, result will be output during the next conversion
		PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch0));

		XPCC_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch1)) << xpcc::endl;
		XPCC_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch2)) << xpcc::endl;

		// Enable auto-shutdown
		adc.setAutoShutdownEnabled(true);

		XPCC_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch2)) << xpcc::endl;

		XPCC_LOG_INFO << "Test single conversion with auto-shutdown (Ch 5):" << xpcc::endl;
		PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch5));
		XPCC_LOG_INFO << PT_CALL(adc.singleConversion(ad7928::InputChannel::Ch0)) << xpcc::endl;

		adc.setAutoShutdownEnabled(false);

		XPCC_LOG_INFO << "Test sequence mode" << xpcc::endl;
		XPCC_LOG_INFO << "Program sequence Ch0,Ch1,Ch5, Ch0,Ch4,Ch5" << xpcc::endl;
		PT_CALL(adc.startSequence(sequence1, sequence2));

		// Run forever
		while (true)
		{
			XPCC_LOG_INFO << PT_CALL(adc.nextSequenceConversion()) << xpcc::endl;

			timeout.restart(500);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::Ad7928<SpiMaster, Cs> adc;
} thread;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Cs::setOutput(xpcc::Gpio::High);

	Sck::connect(SpiMaster::Sck);
	Mosi::connect(SpiMaster::Mosi);
	Miso::connect(SpiMaster::Miso);

	// Initialize the SPI with a 13.5MHz clock (core frequency / 16)
	SpiMaster::initialize<Board::systemClock, 13500000ul>();

	XPCC_LOG_INFO << "AD7928/AD7918/AD7908 Example" << xpcc::endl;

	while (1) {
		thread.update();
	}

	return 0;
}
