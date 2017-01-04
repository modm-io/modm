/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XILINX_SPARTAN3_IMPL_HPP_
#define XILINX_SPARTAN3_IMPL_HPP_

#ifndef XPCC__XILINX_SPARTAN_3
#error "Please include xilinx_spartan3.hpp instead."
#endif

#include <stdint.h>
#include <modm/debug.hpp>

#undef  XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG


template <	typename Cclk,
			typename Din,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource >
void
xpcc::XilinxSpartan3<Cclk, Din, ProgB, InitB, Done, DataSource>::configurePins()
{
	Cclk::setOutput(
			xpcc::stm32::Gpio::OutputType::PushPull,
			xpcc::stm32::Gpio::OutputSpeed::MHz50);
	Cclk::reset();

	Done::setInput(xpcc::stm32::Gpio::InputType::Floating);
	ProgB::setOutput(
			xpcc::stm32::Gpio::OutputType::OpenDrain,
			xpcc::stm32::Gpio::OutputSpeed::MHz50);

	InitB::setInput(xpcc::stm32::Gpio::InputType::PullDown);
	Din::setOutput(
			xpcc::stm32::Gpio::OutputType::PushPull,
			xpcc::stm32::Gpio::OutputSpeed::MHz50);
}

template <	typename Cclk,
			typename Din,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource >
bool
xpcc::XilinxSpartan3<Cclk, Din, ProgB, InitB, Done, DataSource>::configure(const FpgaType fpgaType)
{

	configurePins();

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Configuring FPGA" << xpcc::endl;

	ProgB::reset();

	{
		// wait until InitB and Done go low
		uint32_t counter = 0;
		while (InitB::read() == xpcc::Gpio::High ||
				Done::read() == xpcc::Gpio::High)
		{
			xpcc::delayMicroseconds(1);
			if (counter++ > 1000) {
				// Timeout (1ms) reached, FPGA is not responding abort configuration
				XPCC_LOG_ERROR << XPCC_FILE_INFO;
				XPCC_LOG_ERROR << "FPGA not responding during reset!" << xpcc::endl;
				return false;
			}
		}
	}
	// Led1::reset();

	xpcc::delayMicroseconds(1);
	ProgB::set();

	// Wait until INIT_B goes high
	uint32_t counter = 0;
	while (InitB::read() == xpcc::Gpio::Low)
	{
		xpcc::delayMicroseconds(1);
		if (counter++ > 1000) {
			// Timeout (1ms) reached, FPGA is not responding abort configuration
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA not responding!" << xpcc::endl;
			return false;
		}
	}
	// Led2::reset();

	// wait 0.5..4us before starting the configuration
	xpcc::delayMicroseconds(4);

	uint8_t buffer[256];

	DataSource::readPageFromMemory(0, buffer, sizeof(buffer));

	uint32_t pos = 0;
	uint32_t offset = 0;
	do {
		uint8_t byte = buffer[offset];

		// write byte
		for (uint_fast8_t i = 0; i < 8; i++)
		{
			// MSB first
			if (byte & 0x80) {
				Din::set();
			}
			else {
				Din::reset();
			}
			byte <<= 1;

			Cclk::set();
			Cclk::reset();

			if (Done::read() == xpcc::Gpio::High) {
				XPCC_LOG_DEBUG << "FPGA configuration successful" << xpcc::endl;
				XPCC_LOG_ERROR << "addr=" << pos << xpcc::endl;
				break;
			}

			if (InitB::read() == xpcc::Gpio::Low) {
				// error in configuration
				XPCC_LOG_ERROR << "FPGA configuration aborted!" << xpcc::endl;
				XPCC_LOG_ERROR << "addr=" << pos << xpcc::endl;
				return false;
			}
		}

		offset++;
		pos++;
		if (offset == 256) {
			offset = 0;
			DataSource::readPageFromMemory(pos, buffer, sizeof(buffer));

			// Led3::toggle();
		}

		if (pos > static_cast<uint32_t>(fpgaType) + 100) {
			// More bits written than available
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA configuration failed!" << xpcc::endl;
			return false;
		}
	} while (Done::read() == xpcc::Gpio::Low);
	// Led3::reset();

	// TODO see Xilinx UG332 if there are more clock cycles needed
	for (uint_fast8_t i = 0; i < 10; ++i) {
		Cclk::set();
		Cclk::reset();
	}
	// Led4::reset();

	return true;
}

#endif /* XILINX_SPARTAN3_IMPL_HPP_ */
