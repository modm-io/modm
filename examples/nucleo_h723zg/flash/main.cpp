/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>
#include <modm/board.hpp>

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

extern "C" const uint32_t __flash_reserved_start[];

static Flash::MaxWordType
makeFlashWord(const uint32_t *data)
{
	Flash::MaxWordType word;
	for (size_t ii = 0; ii < Flash::FlashWordWords; ++ii) {
		word.data[ii] = data[ii];
	}
	return word;
}

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "STM32H7 Flash write/read example" << modm::endl;

	constexpr uint32_t dummy_data[] = {
		0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xAABBCCDD,
		0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00,
		0x0BADCAFE, 0xC001D00D, 0xFEEDFACE, 0x8BADF00D,
		0x13579BDF, 0x2468ACE0, 0x10203040, 0x50607080,
	};
	constexpr size_t num_values = std::size(dummy_data);
	static_assert(num_values % Flash::FlashWordWords == 0);
	constexpr size_t num_flash_words = num_values / Flash::FlashWordWords;

	const uintptr_t base = reinterpret_cast<uintptr_t>(__flash_reserved_start);
	const uint8_t sector = Flash::getPage(base - Flash::OriginAddr);

	MODM_LOG_INFO << "Reserved sector: " << sector << "  base: 0x" << modm::hex << base
				  << modm::endl;

	if (not Flash::unlock())
	{
		MODM_LOG_ERROR << "Flash unlock failed!" << modm::endl;
		while (1);
	}

	// Erase the reserved sector before writing
	if (const uint32_t err = Flash::erase(sector); err != 0)
	{
		MODM_LOG_ERROR << "Erase error: 0x" << modm::hex << err << modm::endl;
		while (1);
	}
	MODM_LOG_INFO << "Sector erased." << modm::endl;

	// Write dummy data
	uint32_t err{0};
	for (size_t i = 0; i < num_flash_words; i++)
		err |= Flash::program(base + i * Flash::FlashWordSize,
							  makeFlashWord(&dummy_data[i * Flash::FlashWordWords]));

	if (err != 0)
	{
		MODM_LOG_ERROR << "Program error: 0x" << modm::hex << err << modm::endl;
		while (1);
	}
	MODM_LOG_INFO << "Write complete. Verifying..." << modm::endl;

	// Read back and verify
	for (size_t i = 0; i < num_values; i++)
	{
		uint32_t readback;
		memcpy(&readback, reinterpret_cast<const void *>(base + i * sizeof(uint32_t)),
			   sizeof(uint32_t));
		const bool ok = (readback == dummy_data[i]);
		MODM_LOG_INFO << "[" << i << "] wrote 0x" << modm::hex << dummy_data[i] << "  read 0x"
					  << readback << (ok ? "  OK" : "  MISMATCH") << modm::endl;
	}

	while (1);
	return 0;
}
