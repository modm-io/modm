/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2018, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

typedef GpioInputA0 AdcIn1;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	Adc::connect<AdcIn1::In0>();
	Adc::initialize<Board::SystemClock, Adc::ClockMode::Synchronous, 12_MHz>();

	uint16_t Vref = Adc::readInternalVoltageReference();
	int16_t Temp = Adc::readTemperature(Vref);
	MODM_LOG_INFO << "Vref=" << Vref << modm::endl;
	MODM_LOG_INFO << "Temp=" << Temp << modm::endl;

	MODM_LOG_INFO << "TS_CAL1=" << *Adc::TS_CAL1 << modm::endl;
	MODM_LOG_INFO << "TS_CAL2=" << *Adc::TS_CAL2 << modm::endl;
	MODM_LOG_INFO << "VREFINT_CAL=" << *Adc::VREFINT_CAL << modm::endl;

	Adc::setPinChannel<AdcIn1>();
	Adc::setResolution(Adc::Resolution::Bits12);
	Adc::setRightAdjustResult();
	Adc::setSampleTime(Adc::SampleTime::Cycles239_5);
	Adc::enableFreeRunningMode();
	Adc::startConversion();

	while (true)
	{
		LedD13::toggle();
		modm::delay(100ms);

		MODM_LOG_INFO << "mV=" << (Vref * Adc::getValue() / 4095ul) << modm::endl;

	}

	return 0;
}
