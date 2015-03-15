// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "vl6180.hpp"

namespace xpcc
{

namespace vl6180_private
{

FLASH_STORAGE(BinaryConfiguration configuration[]) =
{
	// Mandatory : private registers (the infamous binary blob of black magic)
	{0x207, 0x01},
	{0x208, 0x01},
	{0x096, 0x00},
	{0x097, 0xfd},
	{0x0e3, 0x00},
	// 5
	{0x0e4, 0x04},
	{0x0e5, 0x02},
	{0x0e6, 0x01},
	{0x0e7, 0x03},
	{0x0f5, 0x02},
	// 10
	{0x0d9, 0x05},
	{0x0db, 0xce},
	{0x0dc, 0x03},
	{0x0dd, 0xf8},
	{0x09f, 0x00},
	// 15
	{0x0a3, 0x3c},
	{0x0b7, 0x00},
	{0x0bb, 0x3c},
	{0x0b2, 0x09},
	{0x0ca, 0x09},
	// 20
	{0x198, 0x01},
	{0x1b0, 0x17},
	{0x1ad, 0x00},
	{0x0ff, 0x05},
	{0x100, 0x05},
	// 25
	{0x199, 0x05},
	{0x1a6, 0x1b},
	{0x1ac, 0x3e},
	{0x1a7, 0x1f},
	{0x030, 0x00},
	// 30

	// Recommended : Public registers
	{0x011, 0x10},	// Enables polling for `New Sample ready` when measurement completes on GPIO1
	{0x10a, 0x30},	// Set the averaging sample period (compromise between lower noise and increased execution time)
	{0x03f, 0x46},	// Sets the light and dark gain (upper nibble). Dark gain should not be changed.
	{0x031, 0xff},	// sets the # of range measurements after which auto calibration of system is performed
	{0x040, 0x00},	// Set ALS integration time to 100ms MSB
	// 35
	{0x041, 0x63},	// Set ALS integration time to 100ms LSB
	{0x02e, 0x01},	// perform a single temperature calibration of the ranging sensor

	// Optional: Public registers
	{0x014, 0x24},	// Configures interrupt on `New Sample Ready threshold event`
	{0x01b, 0x09},	// Set default ranging inter-measurement period to 100ms
	{0x03e, 0x31},	// Set default ALS inter-measurement period to 500ms
	// 40
//	{0x120, 0x01},	// Firmware scaler of 1
//	{0x01c, 0x00},	// Set max convergence time to 30ms MSB
//	{0x01d, 30},	// Set max convergence time to 30ms LSB
//	{0x022, 0x00},	// Early convergence estimate cut off at 80% MSB
//	{0x023, 204},	// Early convergence estimate cut off at 80% LSB
//	// 45
//	{0x02D, 0x11},	// Range check enabled: SNR and ECE
	// 46
};

/// binary scaled gain values (scalar is 1638)
FLASH_STORAGE(float gain[]) =
{
	20,		//0x40
	10.32,	//0x41
	5.21,	//0x42
	2.6,	//0x43
	1.72,	//0x44
	1.28,	//0x45
	1.01,	//0x46
	40		//0x47
};

}	// namespace vl6180_private

}	// namespace xpcc
