/*
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2018, 2021 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/color/tcs3472.hpp>
#include <modm/processing.hpp>

class ThreadOne : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		MODM_LOG_INFO << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(sensor.ping())) { break; }
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO << "Device responded" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.initialize())) { break; }
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO << "Device initialized" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.configure(sensor.Gain::X4, sensor.IntegrationTime::MSEC_101)))
			{
				break;
			}
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO << "Device configured" << modm::endl;

		while (true)
		{
			if (PT_CALL(sensor.readColor()))
			{
				const auto rgb = data.getColor();
				MODM_LOG_INFO << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
			}
			timeout.restart(500ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::tcs3472::Data data;
	modm::Tcs3472<I2cMaster1> sensor{data};
};
ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::LedD13::setOutput();

	I2cMaster1::connect<Board::D14::Sda, Board::D15::Scl>();
	I2cMaster1::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to TCS3472 demo!\n\n";

	modm::ShortPeriodicTimer tmr(500ms);
	while (true)
	{
		one.update();
		if (tmr.execute()) Board::LedD13::toggle();
	}

	return 0;
}
