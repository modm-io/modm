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
#include <tusb.h>
#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/math/utils.hpp>

/* You can test this by updating this firmware with itself:

If you want to use the sanity check:

	scons bin && \
	cp ../../../build/black_pill_f411/usb_fatfs/scons-release/usbfatfs.bin . && \
	echo -n "F411" >> usbfatfs.bin && \
	crc32 usbfatfs.bin | xxd -r -p - >> usbfatfs.bin && \
	mv usbfatfs.bin /Volumes/MODM_USB

Otherwise just copy the file as is and `#define WITH_SANITY_CHECK 0`

	scons bin && \
	cp ../../../build/black_pill_f411/usb_fatfs/scons-release/usbfatfs.bin /Volumes/MODM_USB

*/

#define WITH_SANITY_CHECK 1

// ----------------------------------------------------------------------------
modm_ramcode
void ram_apply(uint8_t pages, const uint8_t *image, uint32_t length)
{
	modm::atomic::Lock l;
	// ONLY RAMCODE FROM HERE ON!!!
	for (auto page{0u}; page <= pages; page++)
		Flash::erase(page);
	for (uintptr_t ptr{0}; ptr < length; ptr += sizeof(Flash::MaxWordType))
		Flash::program(Flash::OriginAddr + ptr, *(Flash::MaxWordType*)(image + ptr));
	NVIC_SystemReset();
}

bool is_valid(FIL *fil, size_t size)
{
#if WITH_SANITY_CHECK
	UINT read;
	// Validate processor type string:
	f_lseek(fil, size-8);
	char type[5];
	f_read(fil, type, 4, &read);
	if (memcmp(type, "F411", 4))
		return false;
	// Validate CRC32 of the entire image
	uint32_t file_crc;
	f_read(fil, &file_crc, 4, &read);
	file_crc = modm::swap(file_crc);
	f_rewind(fil);
	uint32_t crc{modm::math::crc32_init};
	for (FSIZE_t offset{0}; offset < size-4; offset++)
	{
		uint8_t data;
		f_read(fil, &data, 1, &read);
		crc = modm::math::crc32_update(crc, data);
	}
	f_rewind(fil);
	return (~crc == file_crc);
#else
	return true;
#endif
}

void
check_for_update()
{
	// FAT12 max filename length is 8.3, so this is the max:
	static const char *firmware_name = "USBFATFS.BIN";
	static constexpr uint8_t buffer_sector{7};
	// Note: This allocates all FatFs buffers *ON THE STACK*!
	// It may be desirable to allocate them statically for your code!
	if (FATFS fs; f_mount(&fs, "", 0) == FR_OK)
	{
		DIR dj;
		if (FILINFO fno; f_findfirst(&dj, &fno, "", firmware_name) == FR_OK and fno.fname[0])
		{
			if (FIL fil; f_open(&fil, firmware_name, FA_READ) == FR_OK)
			{
				if (is_valid(&fil, fno.fsize))
				{
					Board::Led::set();
					// We first copy the file into the last Flash section of 128kB.
					// It's not guaranteed that the file is stored in FatFs in *one*
					// continous chunk and we cannot access FatFs code in ram_apply!!!
					Flash::unlock();
					Flash::erase(buffer_sector);
					uint32_t dst_addr{uint32_t(Flash::getAddr(buffer_sector))};
					for (FSIZE_t offset{0}; offset < fno.fsize;
					     offset += sizeof(Flash::MaxWordType),
					     dst_addr += sizeof(Flash::MaxWordType))
					{
						Flash::MaxWordType buffer; UINT read;
						f_read(&fil, &buffer, sizeof(Flash::MaxWordType), &read);
						Flash::program(dst_addr, buffer);
					}
					// Jump into RAM and copy from last flash page to first pages
					ram_apply(Flash::getPage(fno.fsize), Flash::getAddr(buffer_sector), fno.fsize);
				}
				f_close(&fil);
			}
		}
		f_closedir(&dj);
		f_mount(NULL, "", 1);
	}
}

void
initializeFatFs()
{
	FATFS fs;
	uint8_t fatfs_buffer[FF_MAX_SS];
	f_mount(&fs, "", 0);

	// initialize ramdisk with Fat12 file system
	MKFS_PARM param{FM_FAT | FM_SFD, 0, 0, 0, 0};
	f_mkfs("", &param, fatfs_buffer, sizeof(fatfs_buffer));
	f_setlabel("MODM_USB");

	f_mount(NULL, "", 1);
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeUsbFs();
	initializeFatFs();
	tusb_init();

	while (true)
	{
		tud_task();

		static modm::PeriodicTimer tmr{1s};
		if (tmr.execute()) check_for_update();
	}
	return 0;
}
