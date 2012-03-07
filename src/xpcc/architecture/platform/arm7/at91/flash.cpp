// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: flash.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "device.h"
#include "flash.hpp"

void
xpcc::at91::Flash::write(uint32_t address, uint8_t* buffer, size_t len)
{
	int wrlen;
	
	// do automatic multi-page writes
	while (len)
	{
		// determine how many bytes to write
		wrlen = (len < AT91C_IFLASH_PAGE_SIZE) ? (len) : (AT91C_IFLASH_PAGE_SIZE);
		
		// write page
		writePage(address, buffer, wrlen);
		
		// increment pointers
		address += wrlen;
		buffer += wrlen;
		
		// decrement remaining buffer size
		len -= wrlen;
	}
}

void
xpcc::at91::Flash::writePage(uint32_t address, uint8_t* buffer, size_t len)
{
	int pageword;
	unsigned long* wrptr = (unsigned long*) address;
	
	// do write cycle
	// copy data to flash location
	for (pageword = 0; pageword < (len / 4); pageword++)
	{
		// do the copy byte-wise because incoming buffer might not be word-aligned
		// NOTE: assuming little-endian source
		*wrptr++ = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0] << 0);
		buffer += 4;
	}
	
	// if the flash address does not begin on page boundary, then we do partial-page programming
	if (address & (AT91C_IFLASH_PAGE_SIZE - 1)) {
		AT91C_BASE_MC->MC_FMR |= AT91C_MC_NEBP;
	}
	else {
		AT91C_BASE_MC->MC_FMR &= ~AT91C_MC_NEBP;
	}
	
	// write flash
	AT91C_BASE_MC->MC_FCR = (0x5A << 24) | 
			(((address / AT91C_IFLASH_PAGE_SIZE) << 8) & AT91C_MC_PAGEN) |
			AT91C_MC_FCMD_START_PROG;
	
	while (!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY)) {
		// wait for flash done/ready
	}
}

void
xpcc::at91::Flash::erase(void)
{
	// erase flash
	AT91C_BASE_MC->MC_FCR = (0x5A << 24) | AT91C_MC_FCMD_ERASE_ALL;
	
	while (!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY)) {
		// wait for flash done/ready
	}
}

bool
xpcc::at91::Flash::isLocked(uint32_t address)
{
	// mask address to size of flash
	address &= (AT91C_IFLASH_SIZE - 1);
	
	// determine the lock state of a flash address/page
	if (AT91C_BASE_MC->MC_FSR &
			(1 << (16 + (address / AT91C_IFLASH_LOCK_REGION_SIZE))))
	{
		return true; // region is locked
	}
	else {
		return false; // region is not locked
	}
}

void
xpcc::at91::Flash::setLock(uint32_t address, bool lock)
{
	// mask address to size of flash
	address &= (AT91C_IFLASH_SIZE - 1);
	
	// since lock bits have a small lifetime (100 programming cycles),
	// check to see if lock bit is already set to requested state
	if (isLocked(address) == lock) {
		return; // lock bit is already set as desired
	}
	
	// program the lock bit
	if (lock) {
		AT91C_BASE_MC->MC_FCR = (0x5A << 24) | 
				(((address / AT91C_IFLASH_PAGE_SIZE) << 8) & AT91C_MC_PAGEN) |
				AT91C_MC_FCMD_LOCK;
	}
	else {
		AT91C_BASE_MC->MC_FCR = (0x5A << 24) | 
				(((address / AT91C_IFLASH_PAGE_SIZE) << 8) & AT91C_MC_PAGEN) |
				AT91C_MC_FCMD_UNLOCK;
	}
	
	while (!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY)) {
		// wait for flash done/ready
	}
}
