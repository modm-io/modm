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

#include <modm/board.hpp>

#include <modm/processing.hpp>
#include <modm/driver/temperature/ltc2984.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/io/iostream.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > device;
modm::IOStream logger(device);

/**
 * Example to demonstrate a modm driver for LTC2984 (or LTC2983) with a Pt100 temperature sensor.
 *
 * This example uses SpiMaster2 of STM32F407
 *
 * SCK  PB13
 * MOSI PB15
 * MISO PB14
 * CS   PB12
 *
 * GND and +3V3 are connected to the temperature measurement system.
 *
 * The schematic shows how to connect the Pt100 and the current sense resistor to the chip:
 *  ---------+
 *           |
 *       CH1 +---------+
 *           |         |
 *           |        +++
 * LTC29783  |        | |
 *   or      |        | | R 2k 0.1%
 * LTC29784  |        | |
 *           |        +++
 *           |         |
 *       CH2 +---------+
 *           |         |
 *       CH3 +---------+
 *           |         |
 *           |        +++
 *           |        | |
 *           |        | | Pt100 sensor (4 wire)
 *           |        | |
 *           |        +++
 *           |         |
 *       CH4 +---------+
 *           |         |
 *       CH5 +---------+
 *           |
 *  ---------+
 *
 */
using Cs = GpioOutputB12;
using Sck = GpioOutputB13;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using SpiMaster = SpiMaster2;

class ThreadOne : public modm::pt::Protothread
{
public:
	ThreadOne() :
		tempSensor()
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		while(!PT_CALL(tempSensor.ping())) {
			logger << "Device not reachable" << modm::endl;
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}


		// Configure the device
		PT_CALL(tempSensor.configureChannel(modm::ltc2984::Channel::Ch2, modm::ltc2984::Configuration::rsense(
												modm::ltc2984::Configuration::Rsense::Resistance_t(2000*1024)
												)));
		PT_CALL(tempSensor.configureChannel(modm::ltc2984::Channel::Ch4, modm::ltc2984::Configuration::rtd(
									 modm::ltc2984::Configuration::SensorType::Pt100,
									 modm::ltc2984::Configuration::Rtd::RsenseChannel::Ch2_Ch1,
									 modm::ltc2984::Configuration::Rtd::Wires::Wire4,
									 modm::ltc2984::Configuration::Rtd::ExcitationMode::Rotation_Sharing,
									 modm::ltc2984::Configuration::Rtd::ExcitationCurrent::Current_500uA,
									 modm::ltc2984::Configuration::Rtd::RtdCurve::European
									 )));
		tempSensor.enableChannel(modm::ltc2984::Configuration::MuxChannel::Ch4);
		PT_CALL(tempSensor.setChannels());

		logger << "Device configured" << modm::endl;


		while (true)
		{
			//PT_CALL(tempSensor.initiateMeasurements());
			PT_CALL(tempSensor.initiateSingleMeasurement(modm::ltc2984::Channel::Ch4));
			stamp = modm::Clock::now();

			// we wait until the conversations are done
			while (PT_CALL(tempSensor.isBusy()))
			{
			}
			logger << "Temperature measurement finished." << modm::endl;

			PT_CALL(tempSensor.readChannel(modm::ltc2984::Channel::Ch4, temp));
			logger << "Temperature: " << temp << modm::endl;

			logger << "Time: " << (modm::Clock::now() - stamp) << modm::endl;

			timeout.restart(1s);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::Ltc2984<SpiMaster, Cs> tempSensor;
	modm::Timeout timeout;
	modm::ltc2984::Data temp;
	modm::Timestamp stamp;
};


ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	// Connect the GPIOs to the SPIs alternate function
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();
	// Initialize the SPI with a 1.3MHz clock
	SpiMaster::initialize<Board::SystemClock, 1.3125_MHz>();

	logger << "\n\nWelcome to LTC2983/LTC2984 demo!\n\n";

	while (true)
	{
		one.update();
		Board::LedOrange::toggle();
	}

	return 0;
}
