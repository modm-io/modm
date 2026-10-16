/*
 * Copyright (c) 2018, Raphael Lehmann
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_TEST_HPP
#define MODM_BLOCK_DEVICE_TEST_HPP

#include <modm/debug/logger.hpp>
#include <cstring>

/**
 * Erases a region of a block device, programs alternating patterns into it
 * and compares what it reads back.
 *
 * @tparam BlockSize	size of one program and read operation in bytes
 * @return `true` if all iterations read back the programmed pattern
 */
template< size_t BlockSize, class Device >
bool
testBlockDevice(Device &device, uint32_t address, uint32_t size, uint16_t iterations)
{
	static uint8_t pattern[BlockSize];
	static uint8_t buffer[BlockSize];

	for (uint16_t iteration = 0; iteration < iterations; iteration++)
	{
		std::memset(pattern, (iteration % 2) ? 0x55 : 0xAA, BlockSize);

		if (not device.erase(address, size)) {
			MODM_LOG_ERROR << "Error: Unable to erase device." << modm::endl;
			return false;
		}
		for (uint32_t offset = 0; offset < size; offset += BlockSize) {
			if (not device.program(pattern, address + offset, BlockSize)) {
				MODM_LOG_ERROR << "Error: Unable to write data." << modm::endl;
				return false;
			}
		}
		for (uint32_t offset = 0; offset < size; offset += BlockSize) {
			if (not device.read(buffer, address + offset, BlockSize)) {
				MODM_LOG_ERROR << "Error: Unable to read data." << modm::endl;
				return false;
			}
			if (std::memcmp(pattern, buffer, BlockSize)) {
				MODM_LOG_ERROR << "Error: Read '" << modm::hex;
				for (const uint8_t byte : buffer) MODM_LOG_ERROR << byte;
				MODM_LOG_ERROR << "' at " << (address + offset) << ", expected '" << pattern[0]
							   << "'." << modm::ascii << modm::endl;
				return false;
			}
		}
		MODM_LOG_INFO << "." << modm::flush;
	}
	MODM_LOG_INFO << modm::endl;
	return true;
}

#endif // MODM_BLOCK_DEVICE_TEST_HPP
