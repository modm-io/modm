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

#ifndef MODM_BLOCK_DEVICE_SPISTACK_FLASH_HPP
#define MODM_BLOCK_DEVICE_SPISTACK_FLASH_HPP

#include <algorithm>
#include <cstdlib>

#include <modm/architecture/interface/block_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * \brief	SpiStack homogenoues memory
 *
 * The `read()`, `erase()`,`program()` and `write()` methodes wait for
 * the chip to finish writing to the flash.
 *
 * \tparam SpiBlockDevice		Base SPI block device of the homogenous stack
 *
 * \ingroup	modm_driver_block_device_spi_stack_flash
 * \author	Rasmus Kleist Hørlyck Sørensen
 */
template <typename SpiBlockDevice, uint8_t DieCount>
class BdSpiStackFlash : public modm::BlockDevice, protected NestedResumable<3>
{
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
	read(uint8_t* buffer, bd_address_t address, bd_size_t size);

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
	program(const uint8_t* buffer, bd_address_t address, bd_size_t size);

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
	*  The blocks are erased prior to being programmed
	*
	*  @param buffer	Buffer of data to write to blocks
	*  @param address	Address of first block to begin writing to
	*  @param size		Size to write in bytes (multiple of read block size)
	*  @return			True on success
	*/
	modm::ResumableResult<bool>
	write(const uint8_t* buffer, bd_address_t address, bd_size_t size);

public:
	/** Check if device is busy
	 *
	 * @return True if any die in the stack is busy.
	 */
	modm::ResumableResult<bool>
	isBusy();

	/** This function can be used in another resumable function
	 * to wait until the flash operation is finished.
	 */
	modm::ResumableResult<void>
	waitWhileBusy();

public:
	static constexpr bd_size_t BlockSizeRead = SpiBlockDevice::BlockSizeRead;
	static constexpr bd_size_t BlockSizeWrite = SpiBlockDevice::BlockSizeWrite;
	static constexpr bd_size_t BlockSizeErase = SpiBlockDevice::BlockSizeErase;
	static constexpr bd_size_t DieSize = SpiBlockDevice::DeviceSize;
	static constexpr bd_size_t DeviceSize = DieCount * DieSize;

private:
	std::ldiv_t dv;
	uint32_t index;
	uint8_t currentDie;
	SpiBlockDevice spiBlockDevice;
};

} // namespace modm

#include "block_device_spistack_flash_impl.hpp"

#endif // MODM_BLOCK_DEVICE_SPISTACK_FLASH_HPP
