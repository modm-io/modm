/*
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/processing.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

typedef GpioInputA7 AdcIn0;
typedef GpioInputA4 AdcIn1;
typedef GpioInputA2 AdcIn2;

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

// the three sensors are mapped: x = ch1, y = ch2, z = ch0
Adc2::Channel sensorMapping[3] = {
		Adc2::Channel::Channel7,
		Adc2::Channel::Channel4,
		Adc2::Channel::Channel2
};
// the results are up to 16 bit wide
uint32_t sensorData[3];
// 3 channels and averages of 100 oversamples
#include <modm/driver/adc/adc_sampler.hpp>
typedef modm::AdcSampler< AdcInterrupt2, 3, 100 > sensors;
modm::ShortTimeout timeout(100);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for MODM_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// initialize Adc2
	Adc2::initialize<Board::systemClock>();
	AdcIn0::connect(Adc2::Channel7);
	AdcIn1::connect(Adc2::Channel4);
	AdcIn2::connect(Adc2::Channel2);

	Adc2::enableInterruptVector(5);
	Adc2::enableInterrupt(Adc2::Interrupt::EndOfRegularConversion);

	sensors::initialize(sensorMapping, sensorData);
	sensors::startReadout();

	while (1)
	{
		if (timeout.isExpired() && sensors::isReadoutFinished())
		{
			uint32_t* data = sensors::getData();
			// send it via UART
			MODM_LOG_INFO <<"x="  << data[0] << modm::endl;
			MODM_LOG_INFO <<"y="  << data[1] << modm::endl;
			MODM_LOG_INFO <<"z="  << data[2] << modm::endl;

			// start another readout
			sensors::startReadout();
			timeout.restart(200);
		}
	}

	return 0;
}
