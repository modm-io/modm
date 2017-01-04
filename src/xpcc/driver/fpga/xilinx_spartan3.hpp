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

#ifndef XPCC__XILINX_SPARTAN_3
#define XPCC__XILINX_SPARTAN_3

#include <modm/architecture.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include "fpga_type.hpp"

namespace xpcc
{
/**
 * Configure an Xilinx Spartan 3 series FPGA in Slave Serial mode.
 *
 * In Slave Serial mode (M[2:0] = <1:1:1>) the configuration data is clocked
 * in using the synchronous serial interface.
 *
 * The configuration data is read from @param DataSource which must provide a
 * readPageFromMemory(pos, buffer, sizeof(buffer) method.
 * Appropriate data sources can be external SPI flashes.
 *
 * @ingroup driver_other
 */
template <	typename Cclk,			///< Clock output to FPGA
			typename Din,			///< Data output to FPGA
			typename ProgB,			///< ProgB output to FPGA
			typename InitB,			///< InitB input from FPGA
			typename Done,			///< Done input from FPGA. FPGA signalises end of configuration.
			typename DataSource	>	///< Source of configuration data, e.g. serial external flash.
class XilinxSpartan3 : public Xilinx
{
public:
	static bool
	configure(FpgaType fpgaType);

protected:
	/**
	 * Set used pins to input / output and speed.
	 *
	 * This can be platform specific ?!
	 */
	static void
	configurePins();

};

}

#include "xilinx_spartan3_impl.hpp"

#endif // XPCC__XILINX_SPARTAN_3
