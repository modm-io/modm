/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "../device.hpp"
#include <modm/platform/clock/rcc.hpp>
#include <modm/architecture/interface/register.hpp>

namespace modm::platform
{

/// @ingroup modm_platform_flash
class Flash
{
public:
	static constexpr uintptr_t OriginAddr{ 0x8000000 };
	static constexpr size_t Size{ 0x200000 };
	static constexpr uint8_t SectorShift = 17U;
	static constexpr size_t SectorSize = (1ul	<< 17);
	static constexpr size_t FlashWord = 8U;

	static inline uint8_t *const Origin{(uint8_t*)OriginAddr};

	enum class
	WordSize : uint32_t
	{
		B8 = 0,
		B16 = FLASH_CR_PSIZE_0,
		B32 = FLASH_CR_PSIZE_1,
	};
public:
	inline static void
	enable()
	{
	}

	inline static void
	disable()
	{
	}

	static bool
	isLocked(uint8_t bank)
	{ 
		if(bank == 1)
		{
			return FLASH->CR1 & FLASH_CR_LOCK;
		}
		if(bank == 2)
		{
			return FLASH->CR2 & FLASH_CR_LOCK;
		}
		return true; 
	}

	static inline bool
	isBusy(uint8_t bank)
	{ 
		if(bank == 1)
		{
			return FLASH->SR1 & FLASH_SR_QW;
		}
		if(bank == 2)
		{
			return FLASH->SR2 & FLASH_SR_QW;
		}
		return false; 
	}

	static bool
	unlock(uint8_t bank);

	static bool
	lock(uint8_t bank);

	static inline uint8_t
	getBank(uint8_t sector)
	{ return sector > 7 ? 2 : 1; }

	static inline uint8_t
	getBank(uintptr_t addr)
	{ return getBank(getSector(addr)); }

	static inline uint8_t
	getSector(uint8_t *ptr)
	{ return getPage(ptr - Flash::Origin); }

	static uint8_t
	getSector(uintptr_t offset)
	{ return getPage(offset); }

	static inline uint8_t
	getPage(uint8_t *ptr)
	{ return getPage(ptr - Flash::Origin); }

	static uint8_t
	getPage(uintptr_t offset);

	static inline uint8_t*
	getAddr(uint8_t sector)
	{ return Origin + getOffset(sector); }

	static uint32_t
	getOffset(uint8_t sector);

	static size_t
	getSize(uint8_t sector);

	static void
	initiateErase(uint8_t sector);

	static uint32_t
	finalizeErase(uint8_t bankId);

	static uint32_t
	erase(uint8_t sector);

	static void
	initiateProgram(uintptr_t addr, uintptr_t data);

	static uint32_t
	finalizeProgram(uint8_t bankId);
};

} // namespace modm::platform