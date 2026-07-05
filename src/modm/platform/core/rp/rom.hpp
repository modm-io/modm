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
#pragma once

#include <modm/platform/device.hpp>

namespace modm::platform
{

/// @ingroup modm_platform_core
/**
 * Give access to functions from masked ROM
 */
struct ROM
{


	static constexpr uint16_t FuncTableOffset = 0x0014;
	static constexpr uint16_t TableLookupOffset = 0x0018;

	using TableLookupFn = void *(*)(const uint16_t *table, uint32_t code);
	static modm_always_inline void *
	getRomPtr(uint16_t romAddress)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
		return reinterpret_cast<void *>(*reinterpret_cast<const uint16_t *>(romAddress));
#pragma GCC diagnostic pop
	}
	template<uint8_t c1, uint8_t c2, typename Signature>
	struct Func
	{
		static constexpr uint32_t Code = uint32_t(c1) | (uint32_t(c2) << 8);
		using Prot = Signature;
		static modm_always_inline Signature
		get()
		{
			auto lookup = reinterpret_cast<TableLookupFn>(getRomPtr(TableLookupOffset));
			auto table = static_cast<const uint16_t *>(getRomPtr(FuncTableOffset));
			return reinterpret_cast<Signature>(lookup(table, Code));
		}
	};

	// Common

	/**
	 * void _connect_internal_flash(void)
	 * Restore all QSPI pad controls to their default state, and connect the SSI to the QSPI pads
	 */
	using connect_internal_flash = Func<'I', 'F', void (*)(void)>;

	/**
	 * void _flash_exit_xip(void)
	 * First set up the SSI for serial-mode operations, then issue the fixed XIP exit sequence
	 * described in Section 2.8.1.2. Note that the bootrom code uses the IO forcing logic to drive
	 * the CS pin, which must be cleared before returning the SSI to XIP mode (e.g. by a call to
	 * _flash_flush_cache). This function configures the SSI with a fixed SCK clock divisor of /6.
	 */
	using flash_exit_xip = Func<'E', 'X', void (*)(void)>;

	/**
	 * void _flash_range_erase(uint32_t addr, size_t count, uint32_t block_size, uint8_t block_cmd)
	 * Erase a count bytes, starting at addr (offset from start of flash). Optionally, pass a block
	 * erase command e.g. D8h block erase, and the size of the block erased by this command — this
	 * function will use the larger block erase where possible, for much higher erase speed. addr
	 * must be aligned to a 4096-byte sector, and count must be a multiple of 4096 bytes.
	 */
	using flash_range_erase = Func<'R', 'E', void (*)(uint32_t, size_t, uint32_t, uint8_t)>;

	/**
	 * void _flash_range_program(uint32_t addr, const uint8_t *data, size_t count)
	 * Program data to a range of flash addresses starting at addr (offset from the start of flash)
	 * and count bytes in size. addr must be aligned to a 256-byte boundary, and count must be a
	 * multiple of 256.
	 */
	using flash_range_program = Func<'R', 'P', void (*)(uint32_t, const uint8_t *, size_t)>;

	/**
	 * void _flash_flush_cache(void)
	 * Flush and enable the XIP cache. Also clears the IO forcing on QSPI CSn, so that the SSI can
	 * drive the flash chip select as normal.
	 */
	using flash_flush_cache = Func<'F', 'C', void (*)(void)>;

	/**
	 * void _flash_enter_cmd_xip(void)
	 * Configure the SSI to generate a standard 03h serial read command, with 24 address bits, upon
	 * each XIP access. This is a very slow XIP configuration, but is very widely supported. The
	 * debugger calls this function after performing a flash erase/programming operation, so that
	 * the freshly-programmed code and data is visible to the debug host, without having to know
	 * exactly what kind of flash device is connected.
	 */
	using flash_enter_cmd_xip = Func<'C', 'X', void (*)(void)>;

	// RP2040 specific

	/**
	 * uint32_t _popcount32(uint32_t value)
	 * Return a count of the number of 1 bits in value.
	 */
	using popcount32 = Func<'P', '3', uint32_t (*)(uint32_t)>;

	/**
	 * uint32_t _reverse32(uint32_t value)
	 * Return the bits of value in the reverse order.
	 */
	using reverse32 = Func<'R', '3', uint32_t (*)(uint32_t)>;

	/**
	 * uint32_t _clz32(uint32_t value)
	 * Return the number of consecutive high order 0 bits of value. If value is zero, returns 32.
	 */
	using clz32 = Func<'L', '3', uint32_t (*)(uint32_t)>;

	/**
	 * uint32_t _ctz32(uint32_t value)
	 * Return the number of consecutive low order 0 bits of value. If value is zero, returns 32.
	 */
	using ctz32 = Func<'T', '3', uint32_t (*)(uint32_t)>;

	/**
	 * uint8_t *_memset(uint8_t *ptr, uint8_t c, uint32_t n)
	 * Sets n bytes start at ptr to the value c and returns ptr.
	 */
	using memset = Func<'M', 'S', uint8_t *(*)(uint8_t *, uint8_t, uint32_t)>;

	/**
	 * uint32_t *_memset4(uint32_t *ptr, uint8_t c, uint32_t n)
	 * Sets n bytes start at ptr to the value c and returns ptr. Note this is a slightly more
	 * efficient variant of _memset that may only be used if ptr is word aligned.
	 */
	using memset4 = Func<'S', '4', uint32_t *(*)(uint32_t *, uint8_t, uint32_t)>;

	/**
	 * uint8_t *_memcpy(uint8_t *dest, uint8_t *src, uint32_t n)
	 * Copies n bytes starting at src to dest and returns dest. The results are undefined if the
	 * regions overlap.
	 */
	using memcpy = Func<'M', 'C', uint32_t *(*)(uint8_t *, const uint8_t *, uint32_t)>;

	/**
	 * uint8_t *_memcpy44(uint32_t *dest, uint32_t *src, uint32_t n)
	 * Copies n bytes starting at src to dest and returns dest. The results are undefined if the
	 * regions overlap. Note this is a slightly more efficient variant of _memcpy that may only be
	 * used if dest and src are word aligned.
	 */
	using memcpy44 = Func<'C', '4', uint32_t *(*)(uint32_t *, const uint32_t *, uint32_t)>;

	/**
	 * void _reset_to_usb_boot(uint32_t gpio_activity_pin_mask, uint32_t disable_interface_mask)
	 * Resets the RP2040 and uses the watchdog facility to re-start in BOOTSEL mode:
	 * gpio_activity_pin_mask is provided to enable an "activity light" via GPIO attached LED for
	 * the USB Mass Storage Device:
	 * - 0 No pins are used as per a cold boot.
	 * - Otherwise a single bit set indicating which GPIO pin should be set to output and raised
	 * whenever there is mass storage activity from the host.
	 * disable_interface_mask may be used to control the exposed USB interfaces:
	 * - 0 To enable both interfaces (as per a cold boot)
	 * - 1 To disable the USB Mass Storage Interface (see Section 2.8.4)
	 * - 2 To disable the USB PICOBOOT Interface (see Section 2.8.5)
	 */
	using reset_to_usb_boot = Func<'U', 'B', void (*)(uint32_t, uint32_t)>;

};

}  // namespace modm::platform
