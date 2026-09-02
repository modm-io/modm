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

#include "flash.hpp"
#include <modm/architecture/interface/interrupt.hpp>

static constexpr uint32_t FLASH_SR_ERR = 0xfffe;

namespace modm::platform
{

MODM_ISR(FLASH)
{
	Flash::interruptHandler();
}

volatile bool Flash::waitingForOpToFinish = false;

bool
Flash::unlock(uint8_t bank)
{
	Flash::enable();
	if (isLocked(bank))
	{
		if(1U == bank)
		{
			FLASH->KEYR1 = 0x45670123;
			FLASH->KEYR1 = 0xCDEF89AB;
		} else if(2U == bank)
		{
			FLASH->KEYR2 = 0x45670123;
			FLASH->KEYR2 = 0xCDEF89AB;
		}
	}
	return not isLocked(bank);
}

bool
Flash::lock(uint8_t bank)
{
	if(1U == bank)
	{
		FLASH->CR1 |= FLASH_CR_LOCK;
	} else if(2U == bank)
	{
		FLASH->CR2 |= FLASH_CR_LOCK;
	}
	Flash::disable();
	return isLocked(bank);
}

modm_ramcode void
Flash::initiateErase(uint8_t index)
{
	uint8_t bank = getBank(index);
	Flash::waitingForOpToFinish = true;
	if(bank == 1)
	{
		FLASH->CR1 = FLASH_CR_START | FLASH_CR_EOPIE | FLASH_CR_SER | (uint32_t)FLASH_CR_PSIZE_1 |
				((index << FLASH_CR_SNB_Pos) & FLASH_CR_SNB_Msk);
		
	} else if(bank == 2)
	{
		FLASH->CR2 = FLASH_CR_START | FLASH_CR_EOPIE | FLASH_CR_SER | (uint32_t)FLASH_CR_PSIZE_1 |
				((index << FLASH_CR_SNB_Pos) & FLASH_CR_SNB_Msk);
	}
}

modm_ramcode uint32_t
Flash::finalizeErase(uint8_t bank)
{
	if(bank == 1)
	{
		FLASH->CR1 = 0;
		return FLASH->SR1 & FLASH_SR_ERR;
	} else if(bank == 2)
	{
		FLASH->CR2 = 0;
		return FLASH->SR2 & FLASH_SR_ERR;
	}
	return 0xffff'ffff;
}
		

modm_ramcode void
Flash::initiateProgram(uintptr_t addr, uintptr_t data)
{
	uint8_t bank = getBank(addr);
	if(1U == bank)
	{
		FLASH->CR1 |= (FLASH_CR_PG | FLASH_CR_EOPIE);
	} else if(2U == bank)
	{
		FLASH->CR2 = (FLASH_CR_PG | FLASH_CR_EOPIE);
	}
	Flash::waitingForOpToFinish = true;
	for(size_t i = 0; i < FlashWord; i++)
	{
		*(uint32_t *) addr = *(uint32_t *)data;
		addr += 4;
		data += 4;
	}
}

modm_ramcode uint32_t
Flash::finalizeProgram(uint8_t bank)
{
	if(1U == bank)
	{
		FLASH->CR1 &= ~(FLASH_CR_PG | FLASH_CR_EOPIE);
		return FLASH->SR1 & FLASH_SR_ERR;
	} else if(2U == bank)
	{
		FLASH->CR2 &= ~(FLASH_CR_PG | FLASH_CR_EOPIE);
		return FLASH->SR2 & FLASH_SR_ERR;
	}
	return 0xFFFF'FFFF;
}

} // namespace modm::platform