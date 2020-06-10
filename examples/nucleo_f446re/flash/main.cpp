/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>

using namespace std::chrono_literals;

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "\n\nReboot\n";
	if (not Flash::unlock()) {
		MODM_LOG_INFO << "Flash unlock failed!" << modm::endl;
	}

	for (uintptr_t offset{0}, sector{255}; offset < Flash::Size; offset += 1)
	{
		const uint8_t nsector = Flash::getSector(offset);
		if (sector != nsector) {
			MODM_LOG_INFO << "Sector " << nsector << " found at boundary " <<
					(Flash::Origin + offset) << modm::endl;
			sector = nsector;
		}
	}

	{
		uint32_t err{0};
		const uint8_t sector_start = Flash::getSector(Flash::Size/2);
		const uint8_t sector_end = Flash::getSector(Flash::Size);
		MODM_LOG_INFO << "Erasing sectors [" << sector_start << ", " << sector_end << ")" << modm::endl;
		MODM_LOG_INFO.flush();
		modm::delay(1s);

		const modm::PreciseTimestamp start = modm::PreciseClock::now();

		for (uint8_t sector{sector_start}; sector < sector_end; sector++)
			err |= Flash::erase(sector);

		const auto diff = (modm::PreciseClock::now() - start);
		MODM_LOG_INFO << "Erasing done in " << diff << " with errors: " << err << modm::endl;
		MODM_LOG_INFO << "Erasing with " << (Flash::Size/2 / (diff.count() >> 10) ) << "kiB/s" << modm::endl;
		MODM_LOG_INFO.flush();
	}

	{
		uint32_t err{0};
		const modm::PreciseTimestamp start = modm::PreciseClock::now();
		for (uint32_t dst_addr{Flash::OriginAddr + Flash::Size/2}, src_addr{Flash::OriginAddr};
		     src_addr < (Flash::OriginAddr + Flash::Size/2);
		     src_addr += sizeof(Flash::MaxWordType), dst_addr += sizeof(Flash::MaxWordType))
		{
			err |= Flash::program(dst_addr, *(Flash::MaxWordType*)src_addr);
		}

		const auto diff = (modm::PreciseClock::now() - start);
		MODM_LOG_INFO << "Programming done in " << diff << " with errors: " << err << modm::endl;
		MODM_LOG_INFO << "Programming with " << (Flash::Size/2 / (diff.count() >> 10) ) << "kiB/s" << modm::endl;
	}

	while(1) ;
	return 0;
}
