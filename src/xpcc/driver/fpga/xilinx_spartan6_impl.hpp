/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XILINX_SPARTAN6_IMPL_HPP_
#define XILINX_SPARTAN6_IMPL_HPP_

#ifndef XPCC__XILINX_SPARTAN_6
#error "Please include xilinx_spartan6.hpp instead."
#endif

#include <stdint.h>
#include <modm/debug/debug.hpp>

#undef  XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

template <	typename Cclk,
			typename DataLow,
			typename DataHigh,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource,
			typename Led0,
			typename Led1 >
bool
xpcc::XilinxSpartan6Parallel<Cclk, DataLow, DataHigh, ProgB, InitB, Done, DataSource, Led0, Led1>::configure(FpgaType fpgaType)
{
	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Configure FPGA" << xpcc::endl;

	Led0::setOutput();
	Led1::setOutput();

	// Reset FPGA
	ProgB::setOutput(xpcc::stm32::Gpio::OutputType::OpenDrain, xpcc::stm32::Gpio::OutputSpeed::MHz50);
	ProgB::reset();

	// TODO what is about alternate function and how to restore?
	InitB::setInput(xpcc::stm32::Gpio::InputType::PullDown);
	Done::setInput(xpcc::stm32::Gpio::InputType::Floating);

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

	// Alternate function wird hiermit auch deaktiviert
	Cclk::setOutput(xpcc::stm32::Gpio::OutputType::PushPull, xpcc::stm32::Gpio::OutputSpeed::MHz50);
	Cclk::reset();

	DataLow::setOutput();
	DataHigh::setOutput();

	Led0::reset();

	xpcc::delayMicroseconds(1);
	// Release reset
	ProgB::set();

	// Wait until INIT_B goes high
	uint32_t counter = 0;
	while (InitB::read() == xpcc::Gpio::Low)
	{
		xpcc::delayMicroseconds(1);
		if (counter++ > 1000) {
			// Timeout (1ms) reached, FPGA is not responding abort configuration
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA not responding after reset!" << xpcc::endl;
			return false;
		}
	}
	Led1::reset();

	uint8_t buffer[256];
	uint32_t pos;

	WritePageState writePageState;
	writePageState.state = writePageState.CONFIG_ONGOING;

	// expected bitstream size is 2742528 bits = 342816 bytes = 1339 pages + 32 bits
	// the last page is not completely used though the clocking should continue
	// for completing internal processes and DONE goes high. Here data is irrelevant.
	// After that additional 8 cycles are needed
	for (pos = 0; writePageState.state == writePageState.CONFIG_ONGOING; pos += sizeof(buffer)) {
		if (pos > static_cast<uint32_t>(fpgaType)) {
			// More bytes written than available
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA configuration failed at position "<< pos << "." << xpcc::endl;
			return false;
		}

		DataSource::readPageFromMemory(pos, buffer, sizeof(buffer));

		Led1::toggle();

		writePage(buffer, writePageState, sizeof(buffer));
	}
	Led1::reset();

	switch (writePageState.state) {
		case WritePageState::CONFIG_DONE:
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG << "FPGA configuration successful" << xpcc::endl;
			XPCC_LOG_DEBUG << "addr = " << pos << xpcc::endl;
			break;
		case WritePageState::CONFIG_CRC_ERROR:
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA configuration aborted with CRC error!" << xpcc::endl;
			XPCC_LOG_ERROR << "addr = " << pos << xpcc::endl;
			return false;
		default:
			XPCC_LOG_ERROR << XPCC_FILE_INFO;
			XPCC_LOG_ERROR << "FPGA configuration unknown state!" << xpcc::endl;
			return false;
	}


	// Apply eight additional CCLK cycles after DONE is asserted High to
	// ensure completion of the FPGA start-up sequence
	for (uint_fast8_t i = 0; i < 8; ++i) {
		Cclk::set();
		Cclk::reset();
	}
	Led0::set();

	return true;
}

template <	typename Cclk,
			typename DataLow,
			typename DataHigh,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource,
			typename Led0,
			typename Led1>
xpcc_always_inline void
xpcc::XilinxSpartan6Parallel<Cclk, DataLow, DataHigh, ProgB, InitB, Done, DataSource, Led0, Led1>::writeWord(uint8_t first, uint8_t second) {
	DataLow::write(first);
	Cclk::set();
	Cclk::reset();
	DataLow::write(second);
	Cclk::set();
	Cclk::reset();
}

template <	typename Cclk,
			typename DataLow,
			typename DataHigh,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource,
			typename Led0,
			typename Led1 >
xpcc_always_inline void
xpcc::XilinxSpartan6Parallel<Cclk, DataLow, DataHigh, ProgB, InitB, Done, DataSource, Led0, Led1>::writePage(uint8_t *buffer, WritePageState& writePageState, uint32_t pageSize) {
	for (uint32_t offset = 0; offset < pageSize; offset+=2) {
		uint8_t first = buffer[offset];
		uint8_t second = buffer[offset+1];

		writeWord(first, second);

		if (Done::read() == xpcc::Gpio::High) {
			writePageState.state = WritePageState::CONFIG_DONE;
			break;
		}

		if (InitB::read() == xpcc::Gpio::Low) {
			writePageState.state = WritePageState::CONFIG_CRC_ERROR;
		}
	}
}

#endif /* XILINX_SPARTAN6_IMPL_HPP_ */
