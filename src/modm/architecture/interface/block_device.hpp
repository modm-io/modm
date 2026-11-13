// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_BLOCK_DEVICE_HPP
#define MODM_INTERFACE_BLOCK_DEVICE_HPP

#include <stdint.h>
#include <algorithm>
#include <cstring>

namespace modm
{

/**
 * Interface of a Block Device
 *
 * Access to storage devices like flash chips
 *
 * @author	Raphael Lehmann
 * @ingroup	modm_architecture_block_device
 */
class BlockDevice
{
public:
	BlockDevice() = default;
	BlockDevice(const BlockDevice&) = delete;
	BlockDevice& operator= (const BlockDevice&) = delete;

	using bd_address_t = uint32_t;
	using bd_size_t = uint32_t;

#ifdef __DOXYGEN__
public:
	/// Initializes the storage hardware
	bool
	initialize();

	/// Deinitializes the storage hardware
	bool
	deinitialize();

	/** Read data from one or more blocks
	 *
	 *  @param buffer	Buffer to read data into
	 *  @param address	Address to begin reading from
	 *  @param size		Size to read in bytes (multiple of read block size)
	 *  @return			True on success. False if any write block could not be
	 *  				read, e.g. with an uncorrectable ECC error, the other
	 *  				blocks are still read. `modm::readMarked()` finds these blocks.
	 */
	[[nodiscard]] bool
	read(uint8_t *buffer, bd_address_t address, bd_size_t size);

	/** Program blocks with data
	 *
	 *  Any block has to be erased prior to being programmed. Unless `ProgramOnce`
	 *  is set, a programmed block can be programmed again to change more bits
	 *  from the `ErasedValue`.
	 *
	 *  @param buffer	Buffer of data to write to blocks
	 *  @param address	Address of first block to begin writing to
	 *  @param size		Size to write in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	[[nodiscard]] bool
	program(const uint8_t *buffer, bd_address_t address, bd_size_t size);

	/** Erase blocks
	 *
	 *  Every byte of an erased block reads as `ErasedValue`.
	 *
	 *  @param address	Address of block to begin erasing
	 *  @param size		Size to erase in bytes, whole erase blocks (see `modm::getEraseBlock()`)
	 *  @return			True on success
	 */
	[[nodiscard]] bool
	erase(bd_address_t address, bd_size_t size);

	/** Writes data to one or more blocks after erasing them
	*
	*  The blocks are erased proir to being programmed
	*
	*  @param buffer	Buffer of data to write to blocks
	*  @param address	Address of first block to begin writing to
	*  @param size		Size to write in bytes (multiple of read block size)
	*  @return			True on success
	*/
	bool
	write(const uint8_t *buffer, bd_address_t address, bd_size_t size);

public:
	/** Minimum block sizes for block device operations
	 *
	 * Most persistent storage devices require `program()`,  `erase()`
	 * and sometimes `read()` operations to be executed on fixed sized blocks.
	 * If the erase blocks differ in size, `BlockSizeErase` is the largest one
	 * and the device implements `EraseBlock getEraseBlock(bd_address_t address)`.
	 */
	static constexpr bd_size_t BlockSizeRead = 1;
	static constexpr bd_size_t BlockSizeWrite = 1;
	static constexpr bd_size_t BlockSizeErase = 1;
	static constexpr bd_size_t DeviceSize = 1'024;

#endif
public:
	/// Value of every byte after `erase()`, 0xff for NOR flash
	static constexpr uint8_t ErasedValue{0xff};

	/// Each write block can only be programmed once after an erase, e.g. on
	/// flash with ECC. Otherwise a block can be programmed again, like NOR
	/// flash without ECC, which only changes bits from the `ErasedValue`.
	static constexpr bool ProgramOnce{false};

	/// Address and size of an erase block
	struct EraseBlock
	{
		bd_address_t address;
		bd_size_t size;
	};
};

/// The erase block of a block device that contains the address. Devices with
/// erase blocks of different sizes implement `getEraseBlock(address)`, the
/// others have erase blocks of `BlockSizeErase` bytes.
/// @ingroup	modm_architecture_block_device
template< class Device >
BlockDevice::EraseBlock
getEraseBlock(const Device &device, BlockDevice::bd_address_t address)
{
	if constexpr (requires { device.getEraseBlock(address); })
		return device.getEraseBlock(address);
	else
		return {address - address % Device::BlockSizeErase, Device::BlockSizeErase};
}

/** Reads data like `device.read()`, but marks write blocks that cannot be read
 *
 *  Every byte of a write block that `read()` fails on, e.g. with an
 *  uncorrectable ECC error after a power loss during programming, is set to
 *  `~ErasedValue`, so that it never looks erased. Storage libraries then find
 *  it with their own CRCs.
 *
 *  @return number of marked write blocks, or -1 for an invalid range
 *  @ingroup	modm_architecture_block_device
 */
template< class Device >
[[nodiscard]] int32_t
readMarked(Device &device, uint8_t *buffer, BlockDevice::bd_address_t address,
		   BlockDevice::bd_size_t size)
{
	using bd_size_t = BlockDevice::bd_size_t;
	if (size == 0 or size % Device::BlockSizeRead or
		address + size > Device::DeviceSize or address + size < address) return -1;
	if (device.read(buffer, address, size)) return 0;
	int32_t marked{0};
	for (bd_size_t pos{0}; pos < size;)
	{
		const bd_size_t block = address + pos - (address + pos) % Device::BlockSizeWrite;
		const bd_size_t end = std::min<bd_size_t>(size, block + Device::BlockSizeWrite - address);
		if (not device.read(buffer + pos, address + pos, end - pos))
		{
			std::memset(buffer + pos, uint8_t(~Device::ErasedValue), end - pos);
			marked++;
		}
		pos = end;
	}
	return marked;
}

}	// namespace modm

#endif // MODM_INTERFACE_BLOCK_DEVICE_HPP
