/*
 * Copyright (c) 2016, Sascha Schade
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

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include "hardware.hpp"

// ----------------------------------------------------------------------------
#include "threads.hpp"

BlinkThread  blinkThread;
Bmp180Thread bmp180Thread;
Bme280Thread bme280Thread;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::IOStream stream(loggerDevice);

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// ----------------------------------------------------------------------------

using namespace Board;

int
main()
{
	Board::initialize();

	// ------------------------------------------------------------------------
	// I2C for sensors
	GpioB6::connect(SensorsAI2cMaster::Scl);
	GpioB7::connect(SensorsAI2cMaster::Sda);
	SensorsAI2cMaster::initialize<Board::systemClock, 10000>();

	GpioB10::connect(SensorsBI2cMaster::Scl);
	GpioB11::connect(SensorsBI2cMaster::Sda);
	SensorsBI2cMaster::initialize<Board::systemClock, 10000>();

	// ------------------------------------------------------------------------
	// initialize Uart2 for MODM_LOG_*
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	MODM_LOG_DEBUG << "Welcome to Environment Sensor Test" << modm::endl;

	LedGreen::set();

	while (true)
	{
		blinkThread.update();
		bmp180Thread.update();
		bme280Thread.update();
	}

	return 0;
}
