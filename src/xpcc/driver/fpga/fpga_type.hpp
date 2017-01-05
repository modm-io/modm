/*
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FPGA_TYPE
#define MODM_FPGA_TYPE

namespace modm
{

struct Xilinx
{
	/*
	 * Device  | Configuration | Configuration
	 *         |  Size [Bits]  |  Size [Bytes]
	 * ------- | ------------- | --------------
	 * XC3S50  |     439,264   |     54,908
	 * XC3S200 |   1,047,616   |    130,952
	 * XC3S400 |   1,699,136   |    212,392
	 * 6SLX9   |   2,742,528   |    342,816
	 *
	 */
	enum class
	FpgaType : uint32_t
	{
		Spartan3XC3S50  =  53908,
		Spartan3XC3S200 = 130952,
		Spartan3XC3S400 = 212392,
		Spartan6SLX9 	= 342816,
	};
};

}

#endif
