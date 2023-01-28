/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>
#include <modm/platform.hpp>

int
main()
{
	Board::initialize();

	using Led = Board::LedGreen;
	Led::setOutput(true);

	Uart0::connect<GpioOutput0::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;
	modm::IOStream out(loggerDevice);

	Adc::connect<GpioInput26::In0, GpioInput27::In1, GpioInput28::In2, GpioInput29::In3>();
	Adc::initialize();
	Adc::enableTemperatureSensor();

	while (true)
	{
		out.printf("---\r\n");
		for (uint8_t ch = 0; ch < (uint8_t)Adc::Channel::Ch3; ++ch)
		{
			auto value = Adc::readChannel((Adc::Channel)ch);
			out.printf("ADC Channel %u %f V | 0x%04x %u\r\n", ch, Adc::convertToVoltage(value),
					   value, value);
		}
		{
			auto value = Adc::readChannel(Adc::Channel::Ch3);
			out.printf("ADC VSYS      %f V | 0x%04x %u\r\n", 3.f * Adc::convertToVoltage(value),
					   value, value);
		}
		{
			auto value = Adc::readChannel(Adc::Channel::Temperature);
			out.printf("ADC Int Temp  %.4f C | 0x%04x %u\r\n", Adc::convertToTemperature(value),
					   value, value);
		}
		modm::delay(250ms);
		Led::toggle();
	}

	return 0;
}
