// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>

#include <modm/processing/processing.hpp>
#include <modm/driver/pressure/amsys5915.hpp>
#include <modm/io/iostream.hpp>
#include <modm/debug/logger.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(device);
modm::log::Logger modm::log::info(device);
modm::log::Logger modm::log::warning(device);
modm::log::Logger modm::log::error(device);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

/**
 * Example to demonstrate a MODM driver for pressure sensor AMSYS 5915
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;
// typedef modm::SoftwareI2cMaster<GpioB10, GpioB11> MyI2cMaster;

modm::amsys5915::Data data;
modm::Amsys5915<MyI2cMaster> pressureSensor(data);

class ThreadOne : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(pressureSensor.ping())) {
			 	break;
			}
			// otherwise, try again in 10ms
			this->timeout.restart(10);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		MODM_LOG_DEBUG << "Device responded" << modm::endl;

		while (true)
		{
			while (! PT_CALL(pressureSensor.readPressure()))
			{
				PT_YIELD();
			}

			MODM_LOG_INFO << "Pressure [0..1]: " << data.getPressure() << modm::endl;
			MODM_LOG_INFO << "Temperature [degree centigrade]: " << data.getTemperature() << modm::endl;

			// read next pressure measurement in 1ms
			this->timeout.restart(1);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}
private:
	modm::ShortTimeout timeout;
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::systemClock, modm::Uart::B115200>(10);

	MyI2cMaster::connect<GpioB10::Scl, GpioB11::Sda>();
	MyI2cMaster::initialize<Board::systemClock, 400000>();

	MODM_LOG_INFO << "\n\nWelcome to AMSYS 5915 pressure sensor demo!\n\n";

	modm::ShortPeriodicTimer tmr(500);

	while (1)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}

	return 0;
}
