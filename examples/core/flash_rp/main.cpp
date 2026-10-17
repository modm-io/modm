/*
 * Copyright (c) 2026, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/platform/flash/flash.hpp>

extern "C" void
putchar_(char c)
{ UsbUart0::write(uint8_t(c)); }

using Flash = modm::platform::Flash;

// Standard Read JEDEC ID instruction: 9Fh command prefix, 24 data bits.
static constexpr uint8_t FLASH_RDID_CMD = 0x9F;
static constexpr size_t FLASH_RDID_DATA_BYTES = 3;
static constexpr size_t FLASH_RDID_TOTAL_BYTES = (1 + FLASH_RDID_DATA_BYTES);

static uint64_t capacity = 0;

static uint32_t
read_jedec_id()
{
	uint8_t tx[FLASH_RDID_TOTAL_BYTES] = {FLASH_RDID_CMD, 0x00, 0x00, 0x00};
	uint8_t rx[FLASH_RDID_TOTAL_BYTES];
	Flash::doCmd(tx, rx, FLASH_RDID_TOTAL_BYTES);
	return (uint32_t(rx[1]) << 16) | (uint32_t(rx[2]) << 8) | rx[3];
}

static void
decode_jedec_id(uint32_t jedec_id)
{

	if (jedec_id == 0x1F8901)
	{
		printf("Found AT25SF128A (16M)\n");
	} else if (jedec_id == 0x9D6014)
	{
		printf("Found IS25LP080D (8M)\n");
	} else if (jedec_id == 0xEF5014)
	{
		printf("Found W25Q80 (1M)\n");
	} else if (jedec_id == 0xEF3011)
	{
		printf("Found W25X10CL (128K)\n");
	} else if (jedec_id == 0x684015)
	{
		printf("Found BY25Q16BS (2M)\n");
	} else if (jedec_id == 0x5e4016)
	{
		printf("Found ZB25VQ32 (4M)\n");
	} else if (jedec_id == 0xef4015)
	{
		printf("Found W25Q16DV (2M)\n");
	} else
	{
		printf("JEDEC code : 0x%06lx\n", jedec_id);
	}
	tud_task();
	uint8_t id3 = jedec_id & 0xFF;
	if (id3 >= 0x10 && id3 <= 0x20)
	{
		capacity = 1llu << id3;
		if (capacity >= (1024 * 1024))
		{
			printf("Decoded Size:  %ld MB\n", uint32_t(capacity / (1024 * 1024)));
		} else
		{
			printf("Decoded Size:  %ld kB\n", uint32_t(capacity / 1024));
		}
	} else
	{
		printf("Decoded Size:    Unknown / Non-standard\n");
	}
	tud_task();
}

static size_t
get_sector()
{
	// use last sector
	return size_t(capacity / Flash::SectorSize) - 1;
}

static size_t
get_page()
{
	// use first page of last sector
	return size_t(capacity / Flash::PageSize) - (Flash::SectorSize / Flash::PageSize);
}

static void
dump_page()
{
	printf("\n");
	const uint8_t* data =
		reinterpret_cast<const uint8_t*>(0x10000000) + get_page() * Flash::PageSize;
	for (size_t i = 0; i < Flash::PageSize; ++i)
	{
		printf("%02x ", data[i]);
		if ((i + 1) % 16 == 0) tud_task();
		if ((i + 1) % 32 == 0) printf("\n");
	}
}

static void
erase_sector()
{
	auto s = get_sector();
	printf("\nErasing sector %x\n", s);
	Flash::eraseSectors(s, 1);
	printf("\nDone\n");
}

static uint8_t data[Flash::PageSize];
static void
program_page()
{
	for (size_t i = 0; i < Flash::PageSize; ++i) { data[i] = i; }
	auto p = get_page();
	printf("\nProgramming page %x\n", p);
	Flash::programPages(p, data, 1);
	printf("\nDone\n");
}

int
main()
{
	Board::initialize();
	Board::initializeUsb();
	tusb_init();

	bool cdc_connected = false;

	while (true)
	{
		if (!cdc_connected && tud_cdc_n_connected(0))
		{
			cdc_connected = true;

			auto jedec_id = read_jedec_id();
			decode_jedec_id(jedec_id);
			tud_task();

			auto id = Flash::getUniqueId();
			printf("Flash unique id: 0x%08lx%08lx\n", uint32_t(id >> 32),
				   uint32_t(id & 0xffffffff));
			tud_task();
			if (capacity)
			{
				printf("'p' - dump page\n");
				tud_task();
				printf("'e' - erase sector\n");
				tud_task();
				printf("'w' - program page\n");
				tud_task();
			}

		} else if (!tud_cdc_n_connected(0))
		{
			cdc_connected = false;
		}

		if (cdc_connected && capacity)
		{
			auto ch = tud_cdc_n_read_char(0);
			if (ch == 'p' || ch == 'P')
			{
				dump_page();
			} else if (ch == 'e' || ch == 'E')
			{
				erase_sector();
			} else if (ch == 'w' || ch == 'W')
			{
				program_page();
			}
		}

		tud_task();
	}

	return 0;
}
