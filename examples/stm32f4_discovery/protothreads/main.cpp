/*
 * Copyright (c) 2014-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/temperature/tmp102.hpp>

#include <modm/io/iostream.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream stream(device);

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
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
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
				stream << "T= " << tI << ".";
				if (tP == 0)
				{
					stream << "0000 C";
				}
				else if (tP == 625)
				{
					stream << "0" << tP << " C";
				}
				else
				{
					stream << tP << " C";
				}
				stream << modm::endl;
				if (result) stream << "Heat me up!" << modm::endl;
			}
			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());
			Board::LedRed::toggle();
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

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, modm::Uart::B38400>(10);

	GpioB7::connect(MyI2cMaster::Sda, Gpio::InputType::PullUp);
	GpioB8::connect(MyI2cMaster::Scl, Gpio::InputType::PullUp);
	MyI2cMaster::initialize<Board::systemClock, 100000>();

	stream << "\n\nRESTART\n\n";

	while (1)
	{
		one.update();
		Board::LedOrange::toggle();
	}

	return 0;
}
