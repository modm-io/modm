/*
 * Copyright (c) 2014-2015, Sascha Schade
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

#include <modm/processing.hpp>
#include <modm/driver/position/vl6180.hpp>
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
 * Example to demonstrate a MODM driver for distance sensor VL6180
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;
// typedef BitBangI2cMaster<GpioB10, GpioB11> MyI2cMaster;

modm::vl6180::Data data;
modm::Vl6180<MyI2cMaster> distance(data);

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
			if (PT_CALL(distance.ping())) {
			 	break;
			}
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		MODM_LOG_DEBUG << "Device responded" << modm::endl;

		while (true)
		{
			if (PT_CALL(distance.initialize()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		MODM_LOG_DEBUG << "Device initialized" << modm::endl;
		this->timeout.restart(1);

		PT_CALL(distance.setIntegrationTime(10));

		while (true)
		{
			stamp = modm::Clock::now();

			if (PT_CALL(distance.readDistance()))
			{
				modm::vl6180::RangeErrorCode error = distance.getRangeError();
				if (error == distance.RangeErrorCode::NoError)
				{
					uint8_t mm = distance.getData().getDistance();
					MODM_LOG_DEBUG << "mm: " << mm;
					Board::LedGreen::set(mm > 160);
					Board::LedBlue::set(mm > 110);
					Board::LedRed::set(mm > 25);
				}
				else {
					MODM_LOG_DEBUG << "Error: " << (uint8_t(error) >> 4);
					Board::LedGreen::set();
					Board::LedBlue::set();
					Board::LedRed::set();
				}
			}

			MODM_LOG_DEBUG << "\tt=" << (modm::Clock::now() - stamp);
			stamp = modm::Clock::now();

			if (PT_CALL(distance.readAmbientLight()))
			{
				modm::vl6180::ALS_ErrorCode error = distance.getALS_Error();
				if (error == distance.ALS_ErrorCode::NoError)
				{
					uint32_t lux = distance.getData().getAmbientLight();
					MODM_LOG_DEBUG << "\tLux: " << lux;
				}
				else {
					MODM_LOG_DEBUG << "\tError: " << (uint8_t(error) >> 4);
				}
			}

			MODM_LOG_DEBUG << " \tt=" << (modm::Clock::now() - stamp) << modm::endl;

			PT_WAIT_UNTIL(this->timeout.isExpired());
			this->timeout.restart(40);
		}

		PT_END();
	}

private:
	modm::Timeout timeout;
	modm::Timestamp stamp;
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, modm::Uart::B115200>();

	MyI2cMaster::connect<GpioB11::Sda, GpioB10::Scl>();
	MyI2cMaster::initialize<Board::systemClock, 400000>();

	MODM_LOG_INFO << "\n\nWelcome to VL6180X demo!\n\n";

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
