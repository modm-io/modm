/*
 * Copyright (c) 2018, Raphael Lehmann
 * Copyright (c) 2020, Benjamin Carrick
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
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
#include <modm/driver/storage/block_device_spistack_flash.hpp>

#include "../block_device_test.hpp"
#include "../integration_test.hpp"

// A 512 MBit stacked die flash chip (W25M512VJ) connected to the Arduino header:
//   Cs = A4 (CN7 pin 17), Mosi = B5 (D22), Miso = B4 (D25), Sck = B3 (D23)
using SpiMaster = SpiMaster1;
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;

constexpr uint32_t BlockSize = 256;
constexpr uint32_t DieSize = 32*1024*1024;
constexpr uint32_t DieCount = 2;
constexpr uint32_t MemorySize = DieCount * DieSize;
constexpr uint32_t TestMemorySize = 4*1024;
// the start and the end of both dies
constexpr uint32_t TestMemoryAddress[] = {0, DieSize - TestMemorySize, DieSize, MemorySize - TestMemorySize};

using BdSpiFlash = modm::BdSpiFlash<SpiMaster, Cs, DieSize>;
modm::BdSpiStackFlash<BdSpiFlash, DieCount> storageDevice;

/**
 * Writes alternating patterns to the start and the end of every die of a
 * stacked flash chip using the `modm::BdSpiStackFlash` block device and
 * compares what it reads back.
 */
int
main()
{
	Board::initialize();
	SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 11_MHz>();

	bool passed = storageDevice.initialize();
	if (not passed) {
		MODM_LOG_ERROR << "Error: Unable to initialize device." << modm::endl;
	}

	for (const uint32_t address : TestMemoryAddress)
	{
		MODM_LOG_INFO << "Testing at " << address << modm::endl;
		passed = passed and testBlockDevice<BlockSize>(storageDevice, address, TestMemorySize, 4);
	}

	return finishTest(passed);
}
