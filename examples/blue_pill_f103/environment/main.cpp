/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
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


using namespace Board;

int
main()
{
	Board::initialize();

	// ------------------------------------------------------------------------
	// I2C for sensors
	SensorsAI2cMaster::connect<GpioB6::Scl, GpioB7::Sda>();
	SensorsAI2cMaster::initialize<Board::SystemClock, 10_kHz>();

	SensorsBI2cMaster::connect<GpioB10::Scl, GpioB11::Sda>();
	SensorsBI2cMaster::initialize<Board::SystemClock, 10_kHz>();

	// ------------------------------------------------------------------------
	// initialize Uart2 for MODM_LOG_*
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

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
