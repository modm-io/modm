/*
 * Copyright (c) 2020, Niklas Hauser
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <cstring>

// Erases the last page of the internal Flash, writes a pattern into it and
// reads it back. Your application must not be large enough to use this page!

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "\n\nReboot\n";

	// some data to write, a multiple of the largest Flash word
	static const uint8_t pattern[64] =
	{
		0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE, 0x12, 0x34, 0x56, 0x78, 0xAA, 0xBB, 0xCC, 0xDD,
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00,
		0x0B, 0xAD, 0xCA, 0xFE, 0xC0, 0x01, 0xD0, 0x0D, 0xFE, 0xED, 0xFA, 0xCE, 0x8B, 0xAD, 0xF0, 0x0D,
		0x13, 0x57, 0x9B, 0xDF, 0x24, 0x68, 0xAC, 0xE0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
	};
	static_assert(sizeof(pattern) % sizeof(Flash::MaxWordType) == 0);

	// The last page or sector exists on every device, no matter how large its Flash is
	const uint8_t page = Flash::getPage(Flash::Size - 1);
	const uint8_t *const base = Flash::getAddr(page);
	MODM_LOG_INFO << "Using page " << page << " at " << base << " of size "
	              << Flash::getSize(page) << modm::endl;

	if (not Flash::unlock())
	{
		MODM_LOG_ERROR << "Flash unlock failed!" << modm::endl;
		while (true) ;
	}

	if (const uint32_t err = Flash::erase(page); err)
	{
		MODM_LOG_ERROR << "Erasing failed with errors: " << err << modm::endl;
		while (true) ;
	}
	MODM_LOG_INFO << "Page erased." << modm::endl;

	uint32_t err{0};
	for (size_t offset{0}; offset < sizeof(pattern); offset += sizeof(Flash::MaxWordType))
	{
		Flash::MaxWordType word;
		memcpy(&word, pattern + offset, sizeof(word));
		err |= Flash::program(uintptr_t(base) + offset, word);
	}
	MODM_LOG_INFO << "Programming done with errors: " << err << modm::endl;

	if (memcmp(base, pattern, sizeof(pattern)) == 0) {
		MODM_LOG_INFO << "Verification successful." << modm::endl;
	} else {
		MODM_LOG_ERROR << "Verification failed!" << modm::endl;
	}

	while (true) ;
	return 0;
}
