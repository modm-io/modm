/*
 * Copyright (c) 2020, Sascha Schade
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>

#include <modm/driver/storage/block_device_spiflash.hpp>

using namespace Board;

modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

Board::w25q16::StorageDevice storageDevice;

void printMemoryContent(const uint8_t* address, std::size_t size) {
	for (std::size_t ii = 0; ii < size; ii++) {
		MODM_LOG_INFO.printf("%x", address[ii]);
	}
}

uint8_t bufferA[Board::w25q16::BlockSize];
uint8_t bufferB[Board::w25q16::BlockSize];
uint8_t bufferC[Board::w25q16::BlockSize];

constexpr uint32_t TestMemorySize = 8*1024;

void doMemoryTest()
{
	LedGreen::set();
	MODM_LOG_INFO << "Starting memory test!" << modm::endl;

	for (uint16_t iteration = 0; iteration < 4; iteration++) {
		uint8_t* pattern = (iteration % 2 == 0) ? bufferA : bufferB;

		if (not RF_CALL_BLOCKING(storageDevice.erase(0, TestMemorySize))) {
			MODM_LOG_INFO << "Error: Unable to erase device.";
			return;
		}

		for (uint32_t ii = 0; ii < TestMemorySize; ii += Board::w25q16::BlockSize) {
			if (not RF_CALL_BLOCKING(storageDevice.program(pattern, ii, Board::w25q16::BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to write data.";
				return;
			}
			MODM_LOG_INFO << ".";
		}

		for (uint32_t ii = 0; ii < TestMemorySize; ii += Board::w25q16::BlockSize) {
			if (not RF_CALL_BLOCKING(storageDevice.read(bufferC, ii, Board::w25q16::BlockSize))) {
				MODM_LOG_INFO << "Error: Unable to read data.";
				return;
			}
			else if (std::memcmp(pattern, bufferC, Board::w25q16::BlockSize)) {
				MODM_LOG_INFO << "ii=" << ii << modm::endl;
				MODM_LOG_INFO << "Error: Read '";
				printMemoryContent(bufferC, Board::w25q16::BlockSize);
				MODM_LOG_INFO << "', expected: '";
				printMemoryContent(pattern, Board::w25q16::BlockSize);
				MODM_LOG_INFO << "'." << modm::endl;
				return;
			}
		}
		MODM_LOG_INFO << "." << modm::endl;
	}

	MODM_LOG_INFO << modm::endl << "Finished!" << modm::endl;
	LedGreen::reset();
}

int
main()
{
	Board::initialize();

	Board::initializeW25q16();

	Usart1::connect<GpioInputA9::Tx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	bool initializeSuccess = false;

	MODM_LOG_INFO << "Erasing complete flash chip... (This may take a while)" << modm::endl;

	if (not RF_CALL_BLOCKING(storageDevice.initialize())) {
		MODM_LOG_INFO << "Error: Unable to initialize device.";
	}
	else if (not RF_CALL_BLOCKING(storageDevice.erase(0, Board::w25q16::MemorySize))) {
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

	while (true)
	{
		if (initializeSuccess and Button::read())
		{
			doMemoryTest();
		}
	}

	return 0;
}
