/*
 * Copyright (c) 2013-2015, 2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>
#include <modm/processing/processing.hpp>

using namespace modm::platform;

// Create a new UART object
typedef modm::platform::SystemClock clock;

#include <modm/io/iostream.hpp>
// Create a IOStream for complex formatting tasks
modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > device;
modm::IOStream output(device);


// the three sensors are mapped: x = ch1, y = ch2, z = ch0
Adc::Channel sensorMapping[3] = {1,2,0};
// 3 channels and averages 32 oversamples
#include <modm/driver/adc/adc_sampler.hpp>
typedef modm::AdcSampler< AdcInterrupt, 3, 32 > sensors;
// the results are up to 16 bit wide
sensors::DataType sensorData[3];

modm::ShortTimeout timeout(100);

int
main()
{
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
	Uart0::initialize<clock, 115200>();

	// Initialize the analog to digital converter
	// With the AVR running at 14.7456Mhz and a prescaler of 128 the
	// ADC is running at 115kHz.
	Adc::initialize<clock, 115000>();
	Adc::setReference(Adc::Reference::InternalVcc);
	Adc::enableInterrupt();

	// Enable interrupts, this is needed for every buffered UART
	enableInterrupts();

	output << "Welcome!" << modm::endl;

	sensors::initialize(sensorMapping, sensorData);
	sensors::startReadout();

	while (1)
	{
		if (timeout.isExpired() && sensors::isReadoutFinished())
		{
			uint16_t* data = sensors::getData();
			// send it via UART
			output <<"x="  << data[0] << modm::endl;
			output <<"y="  << data[1] << modm::endl;
			output <<"z="  << data[2] << modm::endl;

			// start another readout
			sensors::startReadout();
			timeout.restart(200);
		}
	}
}
