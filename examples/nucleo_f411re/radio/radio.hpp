/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/board.hpp>
#include <modm/driver/radio/nrf24.hpp>
#include <modm/processing/timer.hpp>


class ClockUs
{
public:
	typedef uint32_t Type;
	using ClockUsTimer = modm::platform::Timer2;

public:
	template< class SystemClock >
	static void
	enable()
	{
		ClockUsTimer::enable();
		ClockUsTimer::setMode(ClockUsTimer::Mode::UpCounter);
		ClockUsTimer::setPrescaler(SystemClock::Timer2 / 1_MHz);
		ClockUsTimer::setOverflow((uint32_t)-1);
		ClockUsTimer::applyAndReset();

		setTime(0);
		ClockUsTimer::start();
	}

	template< typename TimestampType = modm::Timestamp >
	static TimestampType
	now()
	{ return TimestampType(ClockUsTimer::getValue()); }

	static void
	setTime(const Type time)
	{ ClockUsTimer::setValue(time); }
};


using Nrf1Spi  = SpiMaster3;
using Nrf1Sck  = Board::D3;
using Nrf1Mosi = Board::D4;
using Nrf1Miso = Board::D5;
using Nrf1Csn  = Board::D6;
using Nrf1Ce   = Board::D7;
using Nrf1Irq  = Board::D8;

using Nrf1Phy     = modm::Nrf24Phy<Nrf1Spi, Nrf1Csn, Nrf1Ce>;
using Nrf1Config  = modm::Nrf24Config<Nrf1Phy>;
using Nrf1Data    = modm::Nrf24Data<Nrf1Phy, ClockUs>;


// This must normally be declared in a .cpp file, NOT a header file
MODM_ISR(EXTI9_5) // From PA9
{
	Nrf1Irq::acknowledgeExternalInterruptFlag();
	Board::LedD13::toggle();
	Nrf1Data::interruptHandler();
}

using Nrf2Spi  = SpiMaster2;
using Nrf2Sck  = GpioB13;
using Nrf2Mosi = GpioB15;
using Nrf2Miso = GpioB14;
using Nrf2Csn  = GpioB1;
using Nrf2Ce   = GpioB2;
using Nrf2Irq  = GpioB12;

using Nrf2Phy     = modm::Nrf24Phy<Nrf2Spi, Nrf2Csn, Nrf2Ce>;
using Nrf2Config  = modm::Nrf24Config<Nrf2Phy>;
using Nrf2Data    = modm::Nrf24Data<Nrf2Phy, ClockUs>;

MODM_ISR(EXTI15_10) // From PB12
{
	Nrf2Irq::acknowledgeExternalInterruptFlag();
	Board::LedD13::toggle();
	Nrf2Data::interruptHandler();
}

void inline
initializeSpi(uint8_t instances=0b11)
{
	Board::LedD13::setOutput(modm::Gpio::Low);
	ClockUs::enable<Board::SystemClock>();

	if (instances & 0b01)
	{
		MODM_LOG_INFO << "Initializing SPI #1..." << modm::endl;

		Nrf1Csn::setOutput(modm::Gpio::High);
		Nrf1Ce::setOutput(modm::Gpio::Low);

		Nrf1Spi::connect<Nrf1Sck::Sck, Nrf1Mosi::Mosi, Nrf1Miso::Miso>();
		Nrf1Spi::initialize<Board::SystemClock, 1.5_MHz>();
	}

	if (instances & 0b10)
	{
		MODM_LOG_INFO << "Initializing SPI #2..." << modm::endl;

		Nrf2Csn::setOutput(modm::Gpio::High);
		Nrf2Ce::setOutput(modm::Gpio::Low);

		Nrf2Spi::connect<Nrf2Sck::Sck, Nrf2Mosi::Mosi, Nrf2Miso::Miso>();
		Nrf2Spi::initialize<Board::SystemClock, 1.5_MHz>();
	}
}

static constexpr uint8_t nrf_address1{0x10};
static constexpr uint8_t nrf_address2{0x20};

void inline
initializeNrf(uint8_t instances=0b11, uint8_t address1=nrf_address1, uint8_t address2=nrf_address2, uint8_t channel=20)
{
	initializeSpi(instances);

	if (instances & 0b01)
	{
		MODM_LOG_INFO.printf("Initializing NRF24 #1 with address %d on channel %d...\n", address1, channel);

		Nrf1Data::initialize(0xdeadbeef00, address1, 0xFF);

		Nrf1Config::setChannel(channel);
		Nrf1Config::setAutoRetransmitCount(Nrf1Config::AutoRetransmitCount::Retransmit3);
		Nrf1Config::setAutoRetransmitDelay(Nrf1Config::AutoRetransmitDelay::us500);
		Nrf1Config::setSpeed(Nrf1Config::Speed::MBps1);
		Nrf1Config::setCrc(Nrf1Config::Crc::Crc2Byte);

		Nrf1Irq::setInput(Nrf1Irq::InputType::PullUp);
		Nrf1Irq::setInputTrigger(Nrf1Irq::InputTrigger::FallingEdge);
		Nrf1Irq::enableExternalInterrupt();
		Nrf1Irq::enableExternalInterruptVector(4);
	}
	if (instances & 0b10)
	{
		MODM_LOG_INFO.printf("Initializing NRF24 #2 with address %d on channel %d...\n", address2, channel);

		Nrf2Data::initialize(0xdeadbeef00, address2, 0xFF);

		Nrf2Config::setChannel(channel);
		Nrf2Config::setAutoRetransmitCount(Nrf2Config::AutoRetransmitCount::Retransmit3);
		Nrf2Config::setAutoRetransmitDelay(Nrf2Config::AutoRetransmitDelay::us500);
		Nrf2Config::setSpeed(Nrf2Config::Speed::MBps1);
		Nrf2Config::setCrc(Nrf2Config::Crc::Crc2Byte);

		Nrf2Irq::setInput(Nrf2Irq::InputType::PullUp);
		Nrf2Irq::setInputTrigger(Nrf2Irq::InputTrigger::FallingEdge);
		Nrf2Irq::enableExternalInterrupt();
		Nrf2Irq::enableExternalInterruptVector(5);
	}
}
