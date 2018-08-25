/*
 * Copyright (c) 2014-2017, Niklas Hauser
 * Copyright (c) 2014, 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <inttypes.h>
#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/io/iostream.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/driver/pressure/bmp085.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream stream(device);

/**
 * Example to demonstrate a MODM driver for barometer and
 * thermometer BMP085 and BMP180 (which are compatible).
 *
 * PB9	SDA
 * PB8	SCL
 *
 * GND and +3V3 are connected to the barometer module.
 *
 */

typedef I2cMaster1 MyI2cMaster;

class ThreadOne : public modm::pt::Protothread
{
public:
	ThreadOne() :
		barometer(data, 0x77)
	{
	}

	bool
	update()
	{
		PT_BEGIN()

		stream << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(barometer.ping()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		stream << "Device responded" << modm::endl;

		// Configure the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(barometer.initialize()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		stream << "Device configured" << modm::endl;

		static modm::bmp085::Calibration &cal = data.getCalibration();

		stream << "Calibration data is: ";
		stream.printf(" ac1 %d\n", cal.ac1);
		stream.printf(" ac2 %d\n", cal.ac2);
		stream.printf(" ac3 %d\n", cal.ac3);
		stream.printf(" ac4 %d\n", cal.ac4);
		stream.printf(" ac5 %d\n", cal.ac5);
		stream.printf(" ac6 %d\n", cal.ac6);
		stream.printf(" b1 %d\n", cal.b1);
		stream.printf(" b2 %d\n", cal.b2);
		stream.printf(" mb %d\n", cal.mb);
		stream.printf(" mc %d\n", cal.mc);
		stream.printf(" md %d\n", cal.md);

		while (true)
		{
			static modm::ShortPeriodicTimer timer(250);

			PT_WAIT_UNTIL(timer.execute());

			// Returns when new data was read from the sensor
			PT_CALL(barometer.readout());

			{
				int16_t temp  = data.getTemperature();
				int32_t press = data.getPressure();

				stream.printf("Calibrated temperature in 0.1 degree Celsius is: %" PRId16 "\n",   temp  );
				stream.printf("Calibrated pressure in Pa is                   : %" PRId32 "\n\n", press );
			}
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;

	modm::bmp085::Data data;
	modm::Bmp085<MyI2cMaster> barometer;
};


ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, modm::Uart::B115200>();

	MyI2cMaster::connect<GpioB9::Sda, GpioB8::Scl>();
	MyI2cMaster::initialize<Board::systemClock, MyI2cMaster::Baudrate::Standard>();

	stream << "\n\nWelcome to BMP085 demo!\n\n";

	while (1)
	{
		one.update();
		Board::LedOrange::toggle();
	}

	return 0;
}
