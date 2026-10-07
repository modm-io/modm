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

using namespace Board;

/*
 * This example demonstrates the usage of the ADC peripheral.
 * Connect two potentiometers to A0 and A1 to get reasonable readings or just
 * touch the two pins with your fingers to get ... interesting readings.
 * Make sure you're not too charged up with static electricity!
 */

int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();

	Adc1::connect<GpioA0::In0, GpioA1::In1>();
	Adc1::initialize<Board::SystemClock, Adc1::ClockMode::Asynchronous,
					 24_MHz>();  // 24MHz/160.5 sample time=6.6us (fulfill Ts_temp of 5us)

	Adc1::setResolution(Adc1::Resolution::Bits12);
	Adc1::setSampleTime(Adc1::SampleTime::Cycles160_5);
	Adc1::setRightAdjustResult();
	Adc1::enableOversampling(Adc1::OversampleRatio::x16, Adc1::OversampleShift::Div16);

	while (true)
	{
		Board::LedD13::toggle();
		modm::delay(500ms);

		const uint16_t vref = Adc1::readInternalVoltageReference();
		MODM_LOG_INFO.printf("Vref=%4humV T=%2hu°C A0=%4humV A1=%4humV\n",
							 vref, Adc1::readTemperature(vref),
							 (vref * Adc1::readChannel(Adc1::Channel::In0) / 0xFFF),
							 (vref * Adc1::readChannel(Adc1::Channel::In1) / 0xFFF));
	}
}
