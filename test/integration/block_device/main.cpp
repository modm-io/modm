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

#if __has_include(<modm/board.hpp>)
#include <modm/board.hpp>
#else
#include <modm/platform.hpp>
#endif
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

/**
 * Writes alternating patterns into the block devices that do not need any
 * hardware and compares what it reads back.
 */
int
main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();
#endif
	bool passed = true;

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
