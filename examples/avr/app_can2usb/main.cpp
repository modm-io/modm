/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014-2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * CAN2USB Hardware Revision 1.3
 *
 * An AT90CAN128 and FT245 based CAN sniffer.
 *
 * AT90CAN128 fuse settings:
 *
 * lfuse: 0xfe
 * hfuse: 0xd1
 * efuse: 0xfd
 *
 */

#include <modm/platform.hpp>
#include <modm/driver/usb/ft245.hpp>
#include <modm/debug/logger.hpp>

using namespace modm::platform;

// LED1 Status
typedef GpioOutputD7 LedStatus;

// LED2 RX
typedef GpioOutputG3 LedRxLow;
typedef GpioOutputG4 LedRxHigh;

// LED3 TX
typedef GpioOutputB6 LedTxLow;
typedef GpioOutputB7 LedTxHigh;

// LED4
typedef GpioOutputF7 Led4Low;
typedef GpioOutputF4 Led4High;

// LED5
typedef GpioOutputF6 Led5Low;
typedef GpioOutputF3 Led5High;

// ----------------------------------------------------------------------------
typedef GpioPort<GpioA0, 8> myPort;
typedef GpioInputE7  Rxf;
typedef GpioInputE6  Txe;
typedef GpioOutputG1 Rd;
typedef GpioOutputG0  WrInverted;

typedef GpioInverted<WrInverted> Wr;


typedef modm::Ft245<myPort, Rd, Wr, Rxf, Txe> MyFt;

modm::IODeviceWrapper< MyFt, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG


int
main()
{
	LedStatus::setOutput();
	LedStatus::set();

	LedRxLow::setOutput();
	LedRxHigh::setOutput();

	LedTxLow::setOutput();
	LedTxHigh::setOutput();

	Led4Low::setOutput();
	Led4High::setOutput();

	Led5Low::setOutput();
	Led5High::setOutput();

	LedTxHigh::set();
	LedRxLow::set();
	Led4High::set();
	Led5Low::set();


	Rd::setOutput(modm::Gpio::High);
	WrInverted::setOutput(modm::Gpio::Low);

	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint16_t counter = 0;

	while (1)
	{
		modm::delayMilliseconds(100);
		LedStatus::toggle();
		LedRxHigh::toggle();
		LedRxLow::toggle();
		LedTxHigh::toggle();
		LedTxLow::toggle();
		Led4Low::toggle();
		Led4High::toggle();

		MODM_LOG_DEBUG << "This is debugging " << counter++ << modm::endl;

		uint8_t c;
		if (MyFt::read(c)) {
			Led5Low::toggle();
			Led5High::toggle();
			MODM_LOG_DEBUG << "Received '" << c << "'" << modm::endl;
		}
	}
}
