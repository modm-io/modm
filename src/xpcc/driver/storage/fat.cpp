/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
