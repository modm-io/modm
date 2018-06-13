/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

#include <modm/driver/storage/block_device_spiflash.hpp>

using namespace Board;

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t i = 0; i < size; i++) {
		MODM_LOG_INFO.printf("%x", address[i]);
	}
}

using SpiMaster = SpiMaster1;

// Spi flash chip wiring:
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
// Connect WP and HOLD pins to +3V3
// and of course Vdd to +3V3 and Vss to GND


constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 8*1024*1024;
constexpr uint32_t TestMemorySize = 8*1024;

uint8_t bufferA[BlockSize];
uint8_t bufferB[BlockSize];
uint8_t bufferC[BlockSize];

modm::BdSpiFlash<SpiMaster, Cs, MemorySize> storageDevice;

void doMemoryTest()
{
	LedBlue::set();
	MODM_LOG_INFO << "Starting memory test!" << modm::endl;

	for(uint16_t iteration = 0; iteration < 4; iteration++) {
		uint8_t* pattern = (iteration % 2 == 0) ? bufferA : bufferB;

		if(!RF_CALL_BLOCKING(storageDevice.erase(0, TestMemorySize))) {
			MODM_LOG_INFO << "Error: Unable to erase device.";
			return;
		}

		for(uint32_t i = 0; i < TestMemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.program(pattern, i, BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to write data.";
				return;
			}
			MODM_LOG_INFO << ".";
		}

		for(uint32_t i = 0; i < TestMemorySize; i += BlockSize) {
			if(!RF_CALL_BLOCKING(storageDevice.read(bufferC, i, BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to read data.";
				return;
			}
			else if(std::memcmp(pattern, bufferC, BlockSize)) {
				MODM_LOG_INFO << "i=" << i << modm::endl;
				MODM_LOG_INFO << "Error: Read '";
				printMemoryContent(bufferC, BlockSize);
				MODM_LOG_INFO << "', expected: '";
				printMemoryContent(pattern, BlockSize);
				MODM_LOG_INFO << "'." << modm::endl;
				return;
			}
		}
		MODM_LOG_INFO << "." << modm::endl;
	}

	MODM_LOG_INFO << modm::endl << "Finished!" << modm::endl;
	LedBlue::reset();
}

int
main()
{
	/**
	 * This example/test writes alternating patterns a 64MBit
	 * flash chip (SST26VF064B) attached to SPI0 using the
	 * `modm::BdSpiFlash` block device interface.
	 * The memory content is afterwards read and compared
	 * to the pattern.
	 * Write and read operations are done on 64 byte blocks.
	 *
	 * See above for how to wire the flash chip.
	 */

	// initialize board and SPI
	Board::initialize();
	SpiMaster::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	SpiMaster::initialize<Board::systemClock, 11000000>();

	std::memset(bufferA, 0xAA, BlockSize);
	std::memset(bufferB, 0x55, BlockSize);

	bool initializeSuccess = false;

	MODM_LOG_INFO << "Erasing complete flash chip... (This may take a while)" << modm::endl;

	if(!RF_CALL_BLOCKING(storageDevice.initialize())) {
		MODM_LOG_INFO << "Error: Unable to initialize device.";
	}
	else if(!RF_CALL_BLOCKING(storageDevice.erase(0, MemorySize))) {
		MODM_LOG_INFO << "Error: Unable to erase device.";
	}
	else {
		auto id = RF_CALL_BLOCKING(storageDevice.readId());
		MODM_LOG_INFO << "deviceId=" << id.deviceId << " manufacturerId=" << id.manufacturerId;
		MODM_LOG_INFO << " deviceType=" << id.deviceType << modm::endl;

		MODM_LOG_INFO << "status=" << static_cast<uint8_t>(RF_CALL_BLOCKING(storageDevice.readStatus())) << modm::endl;

		MODM_LOG_INFO << "Press USER button to start the memory test." << modm::endl;
		initializeSuccess = true;
	}

	while (1)
	{
		if(initializeSuccess && Button::read())
		{
			doMemoryTest();
		}
	}

	return 0;
}
