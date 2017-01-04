/*
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_XILINX_SPARTAN_6
#define XPCC_XILINX_SPARTAN_6

#include <modm/architecture/architecture.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include "fpga_type.hpp"

namespace xpcc
{

/**
 * Configure an Xilinx Spartan 6 series FPGA in Slave SelectMAP Mode (parallel) mode.
 *
 * In Slave SelectMAP Mode the configuration data is clocked
 * in using the synchronous parallel interface.
 *
 * The configuration data is read from @param DataSource which must provide a
 * readPageFromMemory(pos, buffer, sizeof(buffer) method.
 * Appropriate data sources can be external SPI flashes.
 *
 * @warning
 * configure() changes the function of all pins used in this template.
 * The user must manually restore the desired function.
 *
 * @TODO
 * DataSource Interface:
 *   - open()
 *   - readByte()
 *   - close()
 *
 * Adapter between SPI flash and DataSource
 *
 * @ingroup driver_other
 */
template <	typename Cclk,			///< Clock output to FPGA
			typename DataLow,		///< Data  output to FPGA, 8 bit parallel port
			typename DataHigh,		///< Data  output to FPGA, 8 bit parallel port
			typename ProgB,			///< ProgB output to FPGA
			typename InitB,			///< InitB input from FPGA
			typename Done,			///< Done  input from FPGA. FPGA signalises end of configuration.
			typename DataSource,	///< Source of configuration data, e.g. serial external flash.
			typename Led0 = xpcc::GpioUnused,
			typename Led1 = xpcc::GpioUnused >
class XilinxSpartan6Parallel : public Xilinx
{
public:
	static bool
	configure(FpgaType fpgaType);

protected:
	struct WritePageState {
		enum {CONFIG_ONGOING, CONFIG_DONE, CONFIG_CRC_ERROR} state;
	};

	static void
	writeWord(uint8_t first, uint8_t second);

	static void
	writePage(uint8_t *buffer, WritePageState& writePageState, uint32_t pageSize);

private:
	static_assert(DataLow::width  == 8, "DataLow should be a 8 bit parallel port");
	static_assert(DataHigh::width == 8, "DataHigh should be a 8 bit parallel port");

};

}

#include "xilinx_spartan6_impl.hpp"

#endif // XPCC_XILINX_SPARTAN_6
