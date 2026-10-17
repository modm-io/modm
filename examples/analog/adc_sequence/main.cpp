/*
 * Copyright (c) 2024, Jörg Ebeling
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <array>

using namespace Board;

/*
 * This example demonstrates the usage of the ADC peripheral.
 * Connect two potentiometers to A0 and A1 to get reasonable readings or just
 * touch the two pins with your fingers to get ... interesting readings.
 * Make sure you're not too charged up with static electricity!
 *
 * NOTE: the readings are raw ADC values and need to be converted to voltage!
 */

std::array<uint16_t, 4> adc_data_buf;

static void
adc_handler()
{
	static uint8_t adc_data_idx = 0;

	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfConversion)
	{
		Adc1::acknowledgeInterruptFlags(Adc1::InterruptFlag::EndOfConversion);
		adc_data_buf.at(adc_data_idx) = Adc1::getValue();
		adc_data_idx++;
	}
	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfSequence)
	{
		Adc1::acknowledgeInterruptFlags(Adc1::InterruptFlag::EndOfSequence);
		adc_data_idx = 0;
	}
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();
	MODM_LOG_INFO << "Board initialized" << modm::endl;

	Adc1::connect<GpioA0::In0, GpioA1::In1>();
	Adc1::initialize<Board::SystemClock, Adc1::ClockMode::Asynchronous,
					 24_MHz>();  // 24MHz/160.5 sample time=6.6us (fulfill Ts_temp of 5us)
	Adc1::setResolution(Adc1::Resolution::Bits12);
	Adc1::setRightAdjustResult();
	Adc1::setSampleTime(Adc1::SampleTime::Cycles160_5);

	const Adc1::Channel sequence[4] = {Adc1::Channel::InternalReference, Adc1::Channel::In0,
									   Adc1::Channel::In1, Adc1::Channel::Temperature};
	Adc1::setChannels(sequence);

	Adc1::enableInterruptVector(15);
	Adc1::enableInterrupt(Adc1::Interrupt::EndOfConversion | Adc1::Interrupt::EndOfSequence);
	AdcInterrupt1::attachInterruptHandler(adc_handler);
	Adc1::enableFreeRunningMode();
	Adc1::startConversion();

	while (true)
	{
		Board::LedD13::toggle();
		modm::delay(500ms);

		MODM_LOG_INFO << "ADC: ";
		for (size_t i = 0; auto value : adc_data_buf)
		{
			MODM_LOG_INFO << i++ << "=" << value << " ";
		}
		MODM_LOG_INFO << modm::endl;
	}
	return 0;
}
