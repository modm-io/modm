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

#include <modm/board.hpp>
#include <modm/driver/storage/block_device_heap.hpp>
#include <modm/driver/storage/block_device_mirror.hpp>

#include "../block_device_test.hpp"
#include "../integration_test.hpp"

#ifdef MODM_OS_HOSTED
#include <modm/driver/storage/block_device_file.hpp>
#include <fstream>

struct FilenameA { static constexpr const char* name = "testA.bin~"; };
struct FilenameB { static constexpr const char* name = "testB.bin~"; };

constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 1024*1024;
#else
// Microcontrollers only have very little memory
constexpr uint32_t BlockSize = 8;
constexpr uint32_t MemorySize = 64;
#endif
constexpr uint16_t Iterations = 10;

/// Heap with 8 byte write blocks and 16 byte erase blocks, whose block at 16
/// cannot be read, like flash with a double ECC error
struct FaultyHeap : modm::BdHeap<64>
{
	static constexpr bd_size_t BlockSizeWrite = 8;
	static constexpr bd_size_t BlockSizeErase = 16;

	[[nodiscard]] bool
	read(uint8_t *buffer, bd_address_t address, bd_size_t size)
	{ return BdHeap::read(buffer, address, size) and (address >= 24 or address + size <= 16); }
};

/// Erased value, erase blocks and marked reads of the block device interface
bool
testErasedAndMarked()
{
	static_assert(modm::BdHeap<64>::ErasedValue == 0xff and not modm::BdHeap<64>::ProgramOnce);
	static FaultyHeap device;
	uint8_t buffer[40];
	bool passed = device.initialize() and device.erase(0, 64);
	// Blocks 0 to 40 are fine, the block at 16 is marked as not erased
	passed &= device.readMarked(buffer, 4, 40) == 1;
	for (size_t ii{0}; ii < 40; ii++)
		passed &= buffer[ii] == ((ii >= 12 and ii < 20) ? 0x00 : 0xff);
	passed &= device.readMarked(buffer, 32, 33) == -1;
	const auto block = device.getEraseBlock(37);
	passed &= block.address == 32 and block.size == 16;
	if (not passed) MODM_LOG_ERROR << "Error: erased value or marked read wrong." << modm::endl;
	return passed;
}

/**
 * Writes alternating patterns into the block devices that do not need any
 * hardware and compares what it reads back.
 */
int
main()
{
	Board::initialize();
	bool passed = true;
	passed &= testErasedAndMarked();

	{
		MODM_LOG_INFO << "modm::BdHeap" << modm::endl;
		static modm::BdHeap<MemorySize> device;
		passed &= device.initialize() and testBlockDevice<BlockSize>(device, 0, MemorySize, Iterations);
	}
	{
		MODM_LOG_INFO << "modm::BdMirror of two modm::BdHeap" << modm::endl;
		static modm::BdMirror<modm::BdHeap<MemorySize>, modm::BdHeap<MemorySize>> device;
		passed &= device.initialize() and testBlockDevice<BlockSize>(device, 0, MemorySize, Iterations);
	}
#ifdef MODM_OS_HOSTED
	// create the empty files
	std::ofstream(FilenameA::name).close();
	std::ofstream(FilenameB::name).close();
	{
		MODM_LOG_INFO << "modm::BdFile" << modm::endl;
		static modm::BdFile<FilenameA, MemorySize> device;
		passed &= device.initialize() and testBlockDevice<BlockSize>(device, 0, MemorySize, Iterations);
	}
	{
		MODM_LOG_INFO << "modm::BdMirror of two modm::BdFile" << modm::endl;
		static modm::BdMirror<modm::BdFile<FilenameA, MemorySize>, modm::BdFile<FilenameB, MemorySize>> device;
		passed &= device.initialize() and testBlockDevice<BlockSize>(device, 0, MemorySize, Iterations);
	}
#endif

	return finishTest(passed);
}
