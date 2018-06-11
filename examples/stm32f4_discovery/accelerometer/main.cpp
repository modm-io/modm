/*
 * Copyright (c) 2014-2018, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015, Martin Esser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/inertial/lis302dl.hpp>
#include <modm/processing.hpp>
#include <modm/math/filter.hpp>


// ST changed the accelerometer in the C revision (MB997C)
// change this to `false`, if you have MB997A or MB997B!
#define REVISION_C true

// if you want to use Software I2c change this to `true`
#define USE_I2C false


namespace lis
{

#if USE_I2C
// I2c Transport Layer
typedef GpioA5 Scl;
typedef GpioA7 Sda;
typedef BitBangI2cMaster<Scl, Sda> I2cMaster;

typedef modm::Lis3TransportI2c< I2cMaster > Transport;
#else
typedef Board::lis3::Transport Transport;
#endif

} // namespace lis3

// Data and Driver object
#if REVISION_C
modm::lis3dsh::Data data;
modm::Lis3dsh< lis::Transport > accel(data);
#else
modm::lis302dl::Data data;
modm::Lis302dl< lis::Transport > accel(data);
#endif


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
			if (PT_CALL(accel.ping()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			Board::LedOrange::set();
			PT_WAIT_UNTIL(this->timeout.isExpired());
			Board::LedOrange::reset();
		}

		// initialize with limited range of ~2.3G
		PT_CALL(accel.configure(accel.Scale::G2, accel.MeasurementRate::Hz400));

		while (true)
		{
			PT_CALL(accel.readAcceleration());

#if REVISION_C
			averageX.update(-accel.getData().getY());
			averageY.update(accel.getData().getX());
#else
			averageX.update(accel.getData().getX());
			averageY.update(accel.getData().getY());
#endif

			Board::LedOrange::set(averageX.getValue() < -0.2);
			Board::LedBlue::set(averageX.getValue() > 0.2);
			Board::LedGreen::set(averageY.getValue() < -0.2);
			Board::LedRed::set(averageY.getValue() > 0.2);

			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::filter::MovingAverage<float, 25> averageX;
	modm::filter::MovingAverage<float, 25> averageY;
};

ReaderThread reader;

int
main()
{
	Board::initialize();

#if USE_I2C
	Board::lis3::Cs::setOutput(modm::Gpio::High);
	Board::lis3::Mosi::setOutput(modm::Gpio::High);

	lis::I2cMaster::connect<lis::Scl::BitBang, lis::Sda::BitBang>(MyI2cMaster::PullUps::Internal);
	lis::I2cMaster::initialize<Board::systemClock, 400000>();
#else
	Board::initializeLis3();
#endif

	while (1)
	{
		reader.update();
	}

	return 0;
}
