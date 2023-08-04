/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <string.h>

#include <fatfs/ff.h>
#include <fatfs/diskio.h>
#include <modm/board.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>
#include <modm/processing.hpp>
#include <modm/platform.hpp>

using namespace Board;
using SpiMaster = SpiMaster1;

// Spi flash chip wiring:
using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;
// Connect WP and HOLD pins to +3V3
// and of course Vdd to +3V3 and Vss to GND

constexpr uint32_t SectorSize = 4096;
constexpr uint32_t SectorCount = 8192;
constexpr uint32_t FlashSize = SectorSize * SectorCount;

modm::BdSpiFlash<SpiMaster, Cs, FlashSize> spiFlash;

// ----------------------------------------------------------------------------

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv == 0)
    {
        if (spiFlash.initialize()) {
            return 0;
        }
    }
    return STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv)
{
    return pdrv ? STA_NOINIT : 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == 0)
    {
        if (spiFlash.read(buff, sector * SectorSize, count * SectorSize)) {
            return RES_OK;
        } else {
            return RES_ERROR;
        }
    }
    return RES_NOTRDY;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == 0)
    {
        if (spiFlash.write(buff, sector * SectorSize, count * SectorSize)) {
            return RES_OK;
        } else {
            return RES_ERROR;
        }
    }
    return RES_NOTRDY;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE ctrl, void *buff)
{
    if (pdrv) return RES_NOTRDY;
    switch (ctrl)
    {
        case CTRL_SYNC: return RES_OK;
        case GET_SECTOR_COUNT: *(LBA_t*)buff = SectorCount;	return RES_OK;
        case GET_SECTOR_SIZE:  *(WORD*) buff = SectorSize;	return RES_OK;
        case GET_BLOCK_SIZE:   *(DWORD*)buff = 64;			return RES_OK;
        default: return RES_PARERR;
    }
}
