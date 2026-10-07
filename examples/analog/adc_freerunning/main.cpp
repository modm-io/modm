/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2018, Carl Treudler
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
// The ADC, its analog input and its clock differ between the devices
#if defined STM32G0
using MyAdc = Adc1;
using AdcInput = GpioB10;
using AdcSignal = AdcInput::In11;
constexpr auto AdcClockMode = MyAdc::ClockMode::Asynchronous;
constexpr auto AdcFrequency = 1_MHz;
constexpr auto AdcSampleTime = MyAdc::SampleTime::Cycles160_5;
#else
using MyAdc = Adc;
using AdcInput = GpioA0;
using AdcSignal = AdcInput::In0;
constexpr auto AdcClockMode = MyAdc::ClockMode::Synchronous;
constexpr auto AdcFrequency = 12_MHz;
constexpr auto AdcSampleTime = MyAdc::SampleTime::Cycles239_5;
#endif

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();
	MyAdc::connect<AdcSignal>();

	MyAdc::initialize<Board::SystemClock, AdcClockMode, AdcFrequency>();

	uint16_t Vref = MyAdc::readInternalVoltageReference();
	int16_t Temp = MyAdc::readTemperature(Vref);
	MODM_LOG_INFO << "Vref=" << Vref << modm::endl;
	MODM_LOG_INFO << "Temp=" << Temp << modm::endl;

	MODM_LOG_INFO << "TS_CAL1=" << *MyAdc::TS_CAL1 << modm::endl;
	MODM_LOG_INFO << "VREFINT_CAL=" << *MyAdc::VREFINT_CAL << modm::endl;

	MyAdc::setPinChannel<AdcInput>();
	MyAdc::setResolution(MyAdc::Resolution::Bits12);
	MyAdc::setRightAdjustResult();
	MyAdc::setSampleTime(AdcSampleTime);
	MyAdc::enableFreeRunningMode();
#ifdef STM32G0
	MyAdc::enableOversampling(MyAdc::OversampleRatio::x256, MyAdc::OversampleShift::Div256);
#endif
	MyAdc::startConversion();

	while (true)
	{
		Board::Leds::toggle();
		modm::delay(100ms);

		MODM_LOG_INFO << "mV=" << (Vref * MyAdc::getValue() / 4095ul) << modm::endl;
	}

	return 0;
}
