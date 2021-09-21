/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;
using namespace modm::literals;

// Create IO wrapper for the debug UART, which is connected to the built-in
// USB debugger's virtual COM port
modm::IODeviceWrapper<Board::DebugUart, modm::IOBuffer::BlockIfFull> debugDevice;
modm::IOStream debugStream(debugDevice);

// Rename because the ::Adc typename defined in the CMSIS header conflicts with
// the name imported from the modm::platform namespace
using AdcDev = modm::platform::Adc;

int
main()
{
	/** This example reads all 6 ADC channels and prints their values to the
	 * debug UART.
	 **/

	Board::initialize();

	AdcDev::initialize<Board::SystemClock>();
	AdcDev::connect<
		GpioA17::Ad,
		GpioA18::Ad,
		GpioA19::Ad,
		GpioA20::Ad,
		GpioB0::Ad,
		GpioB1::Ad>();

	while (true)
	{
		debugStream.printf("ADC Readings: ");
		for(uint32_t i=0; i<6; i++)
		{
			debugStream.printf("%5d ", AdcDev::readChannel(i));
		}
		debugStream.printf("\r\n");

		modm::delay(500ms);
	}
}