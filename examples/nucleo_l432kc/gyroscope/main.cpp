/*
 * Copyright (c) 2014-2018, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015, Martin Esser
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/inertial/l3gd20.hpp>
#include <modm/processing.hpp>
#include <modm/math/filter.hpp>

// Example for L3gd20 gyroscope connected to SPI USART interface

struct UartSpi
{
	using Ck = GpioA8;
	using Tx = GpioA9;
	using Rx = GpioA10;
	using Cs = GpioA11;

	using Master = UartSpiMaster1;
};

using Transport = modm::Lis3TransportSpi<UartSpi::Master, UartSpi::Cs>;

namespace
{
	modm::l3gd20::Data data;
	modm::L3gd20<Transport> gyro{data};
}

class ReaderThread : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(gyro.ping()))
				break;
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			Board::LedD13::set();
			PT_WAIT_UNTIL(timeout.isExpired());
			Board::LedD13::reset();
		}

		PT_CALL(gyro.configure(gyro.Scale::Dps250, gyro.MeasurementRate::Hz380));

		while (true)
		{
			PT_CALL(gyro.readRotation());

			averageX.update(gyro.getData().getX());
			averageY.update(gyro.getData().getY());
			averageZ.update(gyro.getData().getZ());

			MODM_LOG_INFO.printf("x: %.2f, y: %.2f, z: %.2f \n",
								 double(averageX.getValue()),
								 double(averageY.getValue()),
								 double(averageZ.getValue()));

			timeout.restart(50ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::filter::MovingAverage<float, 10> averageX;
	modm::filter::MovingAverage<float, 10> averageY;
	modm::filter::MovingAverage<float, 10> averageZ;
};

ReaderThread reader;

int
main()
{
	Board::initialize();

	UartSpi::Master::connect<UartSpi::Ck::Ck, UartSpi::Tx::Tx, UartSpi::Rx::Rx>();
	UartSpi::Master::initialize<Board::SystemClock, 8_MHz, 0_pct>();

	while (true) {
		reader.update();
	}

	return 0;
}
