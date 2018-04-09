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
#include <modm/processing/resumable.hpp>

/**
 * @ingroup		interface
 * @defgroup	storage	Data storage
 */

namespace modm
{

/**
 * Interface of a Block Device
 *
 * Access to storage devices like flash chips
 *
 * @author	Raphael Lehmann
 * @ingroup	storage
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
	modm::ResumableResult<bool>
	initialize();

	/// Deinitializes the storage hardware
	modm::ResumableResult<bool>
	deinitialize();

	/** Read data from one or more blocks
	 *
	 *  @param buffer	Buffer to read data into
	 *  @param address	Address to begin reading from
	 *  @param size		Size to read in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
	read(uint8_t *buffer, bd_address_t address, bd_size_t size);

	/** Program blocks with data
	 *
	 *  Any block has to be erased prior to being programmed
	 *
	 *  @param buffer	Buffer of data to write to blocks
	 *  @param address	Address of first block to begin writing to
	 *  @param size		Size to write in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
	program(const uint8_t *buffer, bd_address_t address, bd_size_t size);

	/** Erase blocks
	 *
	 *  The state of an erased block is undefined until it has been programmed
	 *
	 *  @param address	Address of block to begin erasing
	 *  @param size		Size to erase in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	modm::ResumableResult<bool>
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
	modm::ResumableResult<bool>
	write(const uint8_t *buffer, bd_address_t address, bd_size_t size);

public:
	/** Minimum block sizes for block device operations
	 *
	 * Most persistent storage devices require `program()`,  `erase()`
	 * and sometimes `read()` operations to be executed on fixed sized blocks.
	 */
	static constexpr bd_size_t BlockSizeRead = 1;
	static constexpr bd_size_t BlockSizeWrite = 1;
	static constexpr bd_size_t BlockSizeErase = 1;
	static constexpr bd_size_t BlockDevice = 1024;

#endif
};

}	// namespace modm

#endif // MODM_INTERFACE_BLOCK_DEVICE_HPP
