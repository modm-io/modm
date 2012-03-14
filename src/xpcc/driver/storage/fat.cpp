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
 */
// ----------------------------------------------------------------------------

#include <fatfs/diskio.h>
#include <fatfs/ffconf.h>

#include "fat.hpp"

// ----------------------------------------------------------------------------
// FatFs Backend
extern "C"
DWORD
get_fattime(void)
{
	return (((2010L - 1980L) << 25) |
			(5L << 21) |
			(30L << 16) |
			(16L << 11) |
			(0 << 5) |
			(0));
}

// ----------------------------------------------------------------------------
#if _VOLUMES == 1

static xpcc::fat::PhysicalVolume * globalVolume;

extern "C"
DSTATUS
disk_initialize(BYTE /*drive*/)
{
	return globalVolume->initialize();
	/*if (diskInterface->initialize()) {
		return RES_OK;
	}
	else {
		return RES_ERROR;
	}*/
}

extern "C"
DSTATUS
disk_status(BYTE /*drive*/)
{
	return globalVolume->getStatus();
}

extern "C"
DRESULT
disk_read(BYTE /*drive*/, BYTE* buffer, DWORD sectorNumber, BYTE sectorCount)
{
	return globalVolume->read(buffer, sectorNumber, sectorCount);
	/*if (diskInterface->read(buffer, sectorNumber, sectorCount)) {
		return RES_OK;
	}
	else {
		return RES_ERROR;
	}*/
}

extern "C"
DRESULT
disk_write(BYTE /*drive*/, const BYTE* buffer, DWORD sectorNumber, BYTE sectorCount)
{
	return globalVolume->write(buffer, sectorNumber, sectorCount);
	/*if (diskInterface->write(buffer, sectorNumber, sectorCount)) {
		return RES_OK;
	}
	else {
		return RES_ERROR;
	}*/
}

extern "C"
DRESULT
disk_ioctl(BYTE /*drive*/, BYTE command, void* buffer)
{
	return globalVolume->ioctl(command, reinterpret_cast<uint32_t *>(buffer));
	//return RES_ERROR;
}

#else
#	error "Only one FatFS drive supported at the moment!"
#endif

// ----------------------------------------------------------------------------
xpcc::fat::FileSystem::FileSystem(PhysicalVolume *volume,
		uint8_t drive)
{
	globalVolume = volume;
	this->fileSystem.drv = drive;
	
	f_mount(drive, &this->fileSystem);
}

xpcc::fat::FileSystem::~FileSystem()
{
	f_mount(this->fileSystem.drv, 0);
}

// ----------------------------------------------------------------------------
xpcc::fat::FileInfo::FileInfo()
{
	info.lfname = 0;
	info.lfsize = 0;
}
