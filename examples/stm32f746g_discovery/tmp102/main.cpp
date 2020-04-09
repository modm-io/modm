/*
 * Copyright (c) 2014, Sascha Schade
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

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/temperature/tmp102.hpp>

typedef I2cMaster1 MyI2cMaster;


class ThreadOne : public modm::pt::Protothread
{
public:
	ThreadOne()
	:	temp(temperatureData, 0x48)
	{
	}

	bool
	update()
	{
		temp.update();

		PT_BEGIN();

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(temp.ping()))
				break;
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}


		PT_CALL(temp.setUpdateRate(200));
		PT_CALL(temp.enableExtendedMode());

		PT_CALL(temp.configureAlertMode(
				modm::tmp102::ThermostatMode::Comparator,
				modm::tmp102::AlertPolarity::ActiveLow,
				modm::tmp102::FaultQueue::Faults6));
		PT_CALL(temp.setLowerLimit(28.f));
		PT_CALL(temp.setUpperLimit(30.f));

		while (true)
		{
			{
				PT_CALL(temp.readComparatorMode(result));
				float temperature = temperatureData.getTemperature();
				uint8_t tI = (int) temperature;
				uint16_t tP = (temperature - tI) * 10000;
				MODM_LOG_INFO << "T= " << tI << ".";
				if (tP == 0)
				{
					MODM_LOG_INFO << "0000 C";
				}
				else if (tP == 625)
				{
					MODM_LOG_INFO << "0" << tP << " C";
				}
				else
				{
					MODM_LOG_INFO << tP << " C";
				}
				if (result) { MODM_LOG_INFO << " Heat me up!"; }
				MODM_LOG_INFO << modm::endl;
			}
			timeout.restart(200ms);
			PT_WAIT_UNTIL(timeout.isExpired());
			Board::LedD13::toggle();
		}

		PT_END();
	}

private:
	bool result;
	modm::ShortTimeout timeout;
	modm::tmp102::Data temperatureData;
	modm::Tmp102<MyI2cMaster> temp;
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::LedD13::setOutput(modm::Gpio::Low);

	MyI2cMaster::connect<Board::D14::Sda, Board::D15::Scl>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nRESTART\n\n";

	while (true)
	{
		one.update();
	}

	return 0;
}
