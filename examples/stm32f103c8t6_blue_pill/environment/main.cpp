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
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::IOStream stream(loggerDevice);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

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
	// initialize Uart2 for XPCC_LOG_*
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	XPCC_LOG_DEBUG << "Welcome to Environment Sensor Test" << xpcc::endl;

	LedGreen::set();

	while (true)
	{
		blinkThread.update();
		bmp180Thread.update();
		bme280Thread.update();
	}

	return 0;
}
