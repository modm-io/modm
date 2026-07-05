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

#include "flash.hpp"

#include <hardware/regs/io_qspi.h>
#include <hardware/regs/ssi.h>
#include <hardware/structs/ioqspi.h>
#include <hardware/structs/pads_qspi.h>
#include <hardware/structs/ssi.h>

#include <modm/platform/core/rom.hpp>

namespace modm::platform
{

// code proted from
// https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/hardware_flash/flash.c

static constexpr size_t FLASH_UNIQUE_ID_SIZE_BYTES = 8;

static constexpr uint8_t FLASH_BLOCK_ERASE_CMD = 0xd8;
static constexpr uint8_t FLASH_SECTOR_ERASE_CMD = 0x20;
// Standard RUID instruction: 4Bh command prefix, 32 dummy bits, 64 data bits.
static constexpr uint8_t FLASH_RUID_CMD = 0x4b;
static constexpr size_t FLASH_RUID_DUMMY_BYTES = 4;
static constexpr size_t FLASH_RUID_DATA_BYTES = FLASH_UNIQUE_ID_SIZE_BYTES;
static constexpr size_t FLASH_RUID_TOTAL_BYTES =
	(1 + FLASH_RUID_DUMMY_BYTES + FLASH_RUID_DATA_BYTES);

static constexpr size_t BOOT2_SIZE_WORDS = 64;

static uint32_t boot2_copyout[BOOT2_SIZE_WORDS];
static bool boot2_copyout_valid = false;

struct flash_hardware_save_state_t
{
	uint32_t qspi_pads[NUM_QSPI_GPIOS];
};


modm_ramcode static void
flash_init_boot2_copyout(void)
{
	if (boot2_copyout_valid) return;
	// todo we may want the option of boot2 just being a free function in
	//      user RAM, e.g. if it is larger than 256 bytes
	const volatile uint32_t *copy_from = reinterpret_cast<uint32_t *>(XIP_BASE);
	for (size_t i = 0; i < BOOT2_SIZE_WORDS; ++i) boot2_copyout[i] = copy_from[i];
	asm inline("" ::: "memory");
	boot2_copyout_valid = true;
}

modm_ramcode static void
flash_enable_xip_via_boot2(void)
{ ((void (*)(void))((intptr_t)boot2_copyout + 1))(); }

modm_ramcode static void
flash_save_hardware_state(flash_hardware_save_state_t *state)
{
	for (size_t i = 0; i < NUM_QSPI_GPIOS; ++i) { state->qspi_pads[i] = pads_qspi_hw->io[i]; }
}

modm_ramcode static void
flash_restore_hardware_state(flash_hardware_save_state_t *state)
{
	for (size_t i = 0; i < NUM_QSPI_GPIOS; ++i) { pads_qspi_hw->io[i] = state->qspi_pads[i]; }
}

modm_ramcode static void
flash_cs_force(bool high)
{
	uint32_t field_val = high ? IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_HIGH
							  : IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_VALUE_LOW;
	hw_write_masked(&ioqspi_hw->io[1].ctrl, field_val << IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_LSB,
					IO_QSPI_GPIO_QSPI_SS_CTRL_OUTOVER_BITS);
}

modm_ramcode void
Flash::eraseBlocks(size_t startBlock, size_t count)
{
	auto connect_internal_flash_func = ROM::connect_internal_flash::get();
	auto flash_exit_xip_func = ROM::flash_exit_xip::get();
	auto flash_range_erase_func = ROM::flash_range_erase::get();
	auto flash_flush_cache_func = ROM::flash_flush_cache::get();

	flash_init_boot2_copyout();
	flash_hardware_save_state_t state;
	flash_save_hardware_state(&state);

	// No flash accesses after this point
	asm inline("" ::: "memory");

	connect_internal_flash_func();
	flash_exit_xip_func();
	flash_range_erase_func(startBlock * BlockSize, count * BlockSize, BlockSize,
						   FLASH_BLOCK_ERASE_CMD);
	flash_flush_cache_func();
	flash_enable_xip_via_boot2();
	flash_restore_hardware_state(&state);
}

modm_ramcode void
Flash::eraseSectors(size_t startSertor, size_t count)
{
	auto connect_internal_flash_func = ROM::connect_internal_flash::get();
	auto flash_exit_xip_func = ROM::flash_exit_xip::get();
	auto flash_range_erase_func = ROM::flash_range_erase::get();
	auto flash_flush_cache_func = ROM::flash_flush_cache::get();

	flash_init_boot2_copyout();
	flash_hardware_save_state_t state;
	flash_save_hardware_state(&state);

	// No flash accesses after this point
	asm inline("" ::: "memory");

	connect_internal_flash_func();
	flash_exit_xip_func();
	flash_range_erase_func(startSertor * SectorSize, count * SectorSize, SectorSize,
						   FLASH_SECTOR_ERASE_CMD);
	flash_flush_cache_func();
	flash_enable_xip_via_boot2();
	flash_restore_hardware_state(&state);
}

modm_ramcode void
Flash::programPages(size_t startPage, const void *data, size_t count)
{
	auto connect_internal_flash_func = ROM::connect_internal_flash::get();
	auto flash_exit_xip_func = ROM::flash_exit_xip::get();
	auto flash_range_program_func = ROM::flash_range_program::get();
	auto flash_flush_cache_func = ROM::flash_flush_cache::get();

	flash_init_boot2_copyout();
	flash_hardware_save_state_t state;
	flash_save_hardware_state(&state);

	asm inline("" ::: "memory");

	connect_internal_flash_func();
	flash_exit_xip_func();
	flash_range_program_func(startPage * PageSize, static_cast<const uint8_t *>(data),
							 count * PageSize);
	flash_flush_cache_func();  // Note this is needed to remove CSn IO force as well as cache
							   // flushing
	flash_enable_xip_via_boot2();

	flash_restore_hardware_state(&state);
}

modm_ramcode void
Flash::doCmd(const void *txData, void *rxData, size_t count)
{
	auto connect_internal_flash_func = ROM::connect_internal_flash::get();
	auto flash_exit_xip_func = ROM::flash_exit_xip::get();
	auto flash_flush_cache_func = ROM::flash_flush_cache::get();

	flash_init_boot2_copyout();
	flash_hardware_save_state_t state;
	flash_save_hardware_state(&state);

	asm inline("" ::: "memory");
	connect_internal_flash_func();
	flash_exit_xip_func();

	flash_cs_force(false);
	size_t tx_remaining = count;
	size_t rx_remaining = count;

	const uint8_t *txbuf = static_cast<const uint8_t *>(txData);
	uint8_t *rxbuf = static_cast<uint8_t *>(rxData);

	// Synopsys SSI version
	// We may be interrupted -- don't want FIFO to overflow if we're distracted.
	const size_t max_in_flight = 16 - 2;
	while (tx_remaining || rx_remaining)
	{
		uint32_t flags = ssi_hw->sr;
		bool can_put = flags & SSI_SR_TFNF_BITS;
		bool can_get = flags & SSI_SR_RFNE_BITS;
		if (can_put && tx_remaining && rx_remaining - tx_remaining < max_in_flight)
		{
			ssi_hw->dr0 = *txbuf++;
			--tx_remaining;
		}
		if (can_get && rx_remaining)
		{
			*rxbuf++ = (uint8_t)ssi_hw->dr0;
			--rx_remaining;
		}
	}
	flash_cs_force(true);

	flash_flush_cache_func();
	flash_enable_xip_via_boot2();
	flash_restore_hardware_state(&state);
}

modm_ramcode void
Flash::flush()
{
	auto func = ROM::flash_flush_cache::get();
	func();
}

uint64_t
Flash::getUniqueId()
{
	union {
		uint64_t id;
		uint8_t d[FLASH_RUID_DATA_BYTES];
	} id_out;
	uint8_t txbuf[FLASH_RUID_TOTAL_BYTES] = {0};
	uint8_t rxbuf[FLASH_RUID_TOTAL_BYTES] = {0};
	txbuf[0] = FLASH_RUID_CMD;
	doCmd(txbuf, rxbuf, FLASH_RUID_TOTAL_BYTES);
	for (size_t i = 0; i < FLASH_RUID_DATA_BYTES; i++)
		id_out.d[i] = rxbuf[i + 1 + FLASH_RUID_DUMMY_BYTES];
	return id_out.id;
}

}  // namespace modm::platform
