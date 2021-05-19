/*
 * Copyright (c) 2021, Thomas Sommer
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

using namespace modm::platform;

class Sensorthread : public modm::pt::Protothread
{
private:
	modm::ShortTimeout timeout;

	modm::tcs3472::Data data;
	modm::Tcs3472<I2cMaster> sensor{data};
	using TCS3472_INT = Board::D2;

public:
	bool
	update()
	{
		PT_BEGIN();

		TCS3472_INT::setInput(Gpio::InputType::PullUp);

		MODM_LOG_INFO << "Ping TCS34725" << modm::endl;
		// ping the device until it responds
		while (true)
		{
			// we wait until the task started
			if (PT_CALL(sensor.ping())) { break; }
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		MODM_LOG_INFO << "TCS34725 responded" << modm::endl;

		PT_CALL(sensor.initialize(sensor.Enable_InterruptMode_Waittime));
		PT_CALL(sensor.configure(modm::tcs3472::Gain::X16, modm::tcs3472::IntegrationTime::MSEC_2_4));
		PT_CALL(sensor.setInterruptPersistenceFilter(modm::tcs3472::InterruptPersistence::CNT_20));
		// Setup WaitTime to further slow down samplerate
		PT_CALL(sensor.setWaitTime(modm::tcs3472::WaitTime::MSEC_2_4));

		// Dummy read required
		PT_CALL(sensor.readColor());
		// Fetch one sample ...
		PT_CALL(sensor.readColor());
		// ...and set the high threshold 20% above current clear
		PT_CALL(sensor.setInterruptHighThreshold(data.getClear() * 1.2));

		while (true)
		{
			PT_CALL(sensor.reloadInterrupt());
			PT_WAIT_UNTIL(TCS3472_INT::read() == false);
			if (PT_CALL(sensor.readColor()))
			{
				const auto rgb = data.getColor();
				MODM_LOG_INFO << "RGB: " << rgb << "\tHSV: " << modm::color::Hsv(rgb) << modm::endl;
			}
		}

		PT_END();
	}
};

Sensorthread sensorthread;

int
main()
{
	Board::initialize();
	I2cMaster::initialize<Board::SystemClock, 100_kHz>();

	LedD13::setOutput();
	modm::ShortPeriodicTimer heartbeat(500ms);

	while (true)
	{
		sensorthread.update();
		if (heartbeat.execute()) Board::LedD13::toggle();
	}
}
