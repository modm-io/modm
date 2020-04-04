/*
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
int
main()
{
	Board::initialize();
	LedD13::setOutput();
	Adc1::connect<GpioB10::In11>();

	Adc1::initialize<Board::SystemClock, Adc1::ClockMode::Asynchronous, 1_MHz>();

	uint16_t Vref = Adc1::readInternalVoltageReference();
	int16_t Temp = Adc1::readTemperature(Vref);
	MODM_LOG_INFO << "Vref=" << Vref << modm::endl;
	MODM_LOG_INFO << "Temp=" << Temp << modm::endl;

	MODM_LOG_INFO << "TS_CAL1=" << uint16_t(*((volatile uint16_t *)0x1FFF75A8)) << modm::endl;
	MODM_LOG_INFO << "TS_CAL2=" << uint16_t(*((volatile uint16_t *)0x1FFF75CA)) << modm::endl;
	MODM_LOG_INFO << "VREFINT_CAL=" << uint16_t(*((volatile uint16_t *)0x1FFF75AA)) << modm::endl;
	MODM_LOG_INFO << "ADC_CALFACT=" << uint16_t(ADC1->CALFACT) << modm::endl;

	Adc1::setPinChannel<GpioB10>();
	Adc1::setResolution(Adc1::Resolution::Bits12);
	Adc1::setRightAdjustResult();
	Adc1::setSampleTime(Adc1::SampleTime::Cycles160_5);
	Adc1::enableFreeRunningMode();
	Adc1::enableOversampling(Adc1::OversampleRatio::x256, Adc1::OversampleShift::Div256);
	Adc1::startConversion();

	while (true)
	{
		LedD13::toggle();
		modm::delay(100ms);

		MODM_LOG_INFO << "mV=" << (Vref * Adc1::getValue() / 4095ul) << modm::endl;
	}

	return 0;
}
