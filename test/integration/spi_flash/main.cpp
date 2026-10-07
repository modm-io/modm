/*
 * Copyright (c) 2018, Raphael Lehmann
 * Copyright (c) 2020, Benjamin Carrick
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>

#include "../block_device_test.hpp"
#include "../integration_test.hpp"

#ifdef ONBOARD_FLASH
// The 16 MBit flash chip (W25Q16) on the board
using StorageDevice = Board::w25q16::StorageDevice;
constexpr uint32_t BlockSize = Board::w25q16::BlockSize;

static void
initializeSpi()
{
	Board::initializeW25q16();
}
#else
// A 64 MBit flash chip (SST26VF064B) connected to the Arduino header:
//   Cs = A4 (CN7 pin 17), Mosi = B5 (D22), Miso = B4 (D25), Sck = B3 (D23)
using SpiMaster = SpiMaster1;
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 8*1024*1024;
using StorageDevice = modm::BdSpiFlash<SpiMaster, Cs, MemorySize>;

static void
initializeSpi()
{
	SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 11_MHz>();
}
#endif

constexpr uint32_t TestMemorySize = 8*1024;
StorageDevice storageDevice;

/**
 * Writes alternating patterns into the first 8kB of a SPI flash chip using the
 * `modm::BdSpiFlash` block device and compares what it reads back.
 */
int
main()
{
	Board::initialize();
	initializeSpi();

	bool passed = storageDevice.initialize();
	if (passed)
	{
		const auto id = storageDevice.readId();
		MODM_LOG_INFO << "deviceId=" << id.deviceId << " manufacturerId=" << id.manufacturerId
					  << " deviceType=" << id.deviceType << modm::endl;
		// a missing chip reads as all ones or all zeros
		passed = (id.manufacturerId != 0x00) and (id.manufacturerId != 0xff);
	}
	else {
		MODM_LOG_ERROR << "Error: Unable to initialize device." << modm::endl;
	}

	passed = passed and testBlockDevice<BlockSize>(storageDevice, 0, TestMemorySize, 4);

	return finishTest(passed);
}
