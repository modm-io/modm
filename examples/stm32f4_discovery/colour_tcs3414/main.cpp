/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2018, Niklas Hauser
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
#include <modm/driver/color/tcs3414.hpp>

#include <modm/io/iostream.hpp>

#include <modm/architecture/interface/gpio.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream stream(device);

/**
 * Example to demonstrate a MODM driver for colour sensor TCS3414
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA PB11
 * SCL PB10
 *
 * GND and +3V3 are connected to the colour sensor.
 *
 */

// typedef I2cMaster1 MyI2cMaster;
typedef I2cMaster2 MyI2cMaster;
// typedef BitBangI2cMaster<GpioB10, GpioB11> MyI2cMaster;

class ThreadOne : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		stream << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(sensor.ping())) {
			 	break;
			}
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		stream << "Device responded" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.initialize())) {
				break;
			}
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		stream << "Device initialized" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.configure(
					modm::tcs3414::Gain::X16,
					modm::tcs3414::Prescaler::D1)))
			{
				if (PT_CALL(sensor.setIntegrationTime(
						modm::tcs3414::IntegrationMode::INTERNAL,
						modm::tcs3414::NominalIntegrationTime::MSEC_100)))
				{
					break;
				}
				break;
			}
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		stream << "Device configured" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.readColor())) {
				const auto rgb = data.getColor();
				stream << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
			}
			timeout.restart(500ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::tcs3414::Data data;
	modm::Tcs3414<MyI2cMaster> sensor{data};
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	MyI2cMaster::connect<GpioB11::Sda, GpioB10::Scl>();
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	stream << "\n\nWelcome to TCS3414 demo!\n\n";

	modm::ShortPeriodicTimer tmr(500ms);

	while (true)
	{
		one.update();
		if (tmr.execute()) {
			Board::LedOrange::toggle();
		}
	}

	return 0;
}
