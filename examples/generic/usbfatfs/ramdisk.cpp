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

#include <fatfs/ff.h>
#include <fatfs/diskio.h>
#include <string.h>
#include <modm/architecture/utils.hpp>

// ----------------------------------------------------------------------------
static constexpr uint32_t sector_size{512};
static constexpr uint32_t sector_count{230};
// Allocate giant array inside the SRAM1 noinit section
modm_section(".noinit_sram1") uint8_t ram_disk[sector_count * sector_size];

DSTATUS disk_initialize(BYTE pdrv) { return pdrv ? STA_NOINIT : 0; }
DSTATUS disk_status(BYTE pdrv) { return pdrv ? STA_NOINIT : 0; }
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
	if (pdrv) return RES_NOTRDY;
	memcpy(buff, ram_disk + sector * sector_size, count * sector_size);
	return RES_OK;
}
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
	if (pdrv) return RES_NOTRDY;
	memcpy(ram_disk + sector * sector_size, buff, count * sector_size);
	return RES_OK;
}
DRESULT disk_ioctl(BYTE pdrv, BYTE ctrl, void *buff)
{
	if (pdrv) return RES_NOTRDY;
	switch (ctrl)
	{
		case CTRL_SYNC: return RES_OK;
		case GET_SECTOR_COUNT: *(LBA_t*)buff = sector_count;	return RES_OK;
		case GET_SECTOR_SIZE:  *(WORD*) buff = sector_size;		return RES_OK;
		case GET_BLOCK_SIZE:   *(DWORD*)buff = 64;				return RES_OK;
		default: return RES_PARERR;
	}
}

// ----------------------------------------------------------------------------
// TinyUSB MSC callbacks also accessing the same RAM disk
#include <tusb.h>

void tud_msc_inquiry_cb(uint8_t, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	const char vid[] = "TinyUSB";
	const char pid[] = "Mass Storage";
	const char rev[] = "1.0";
	memcpy(vendor_id  , vid, strlen(vid));
	memcpy(product_id , pid, strlen(pid));
	memcpy(product_rev, rev, strlen(rev));
}
bool tud_msc_test_unit_ready_cb(uint8_t) { return true; }
bool tud_msc_start_stop_cb(uint8_t, uint8_t, bool, bool) { return true; }
void tud_msc_capacity_cb(uint8_t, uint32_t* block_count, uint16_t* block_size)
{
	*block_count = sector_count;
	*block_size  = sector_size;
}
int32_t tud_msc_read10_cb(uint8_t, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	uint8_t const* addr = ram_disk + sector_size * lba + offset;
	memcpy(buffer, addr, bufsize);
	return bufsize;
}
int32_t tud_msc_write10_cb(uint8_t, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	uint8_t* addr = ram_disk + sector_size * lba + offset;
	memcpy(addr, buffer, bufsize);
	return bufsize;
}

int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	void const* response = NULL;
	uint16_t resplen = 0;
	bool in_xfer = true;
	switch (scsi_cmd[0])
	{
		case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
			resplen = 0;
			break;

		default:
			tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
			resplen = -1;
			break;
	}
	if (resplen > bufsize) resplen = bufsize;
	if (response and (resplen > 0) and in_xfer)
		memcpy(buffer, response, resplen);
	return resplen;
}
