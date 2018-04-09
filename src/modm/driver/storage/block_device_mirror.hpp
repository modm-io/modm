// coding: utf-8
/* Copyright (c) 2018, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_MIRROR_HPP
#define MODM_BLOCK_DEVICE_MIRROR_HPP

#include <modm/architecture/interface/block_device.hpp>
#include <modm/processing/resumable.hpp>
#include <algorithm>

namespace modm
{

/**
 * \brief	Virtual block device consists of two mirrored block devices.
 *
 * Write operations (`erase()`, `program()` and `write()`) are forwarded
 * to both block devices.
 * Read operations (`read()`) are only performed on BlockDeviceA.
 *
 * \tparam BlockDeviceA		First block device of the mirrored block devices
 * \tparam BlockDeviceB		Second block device
 *
 * \ingroup	driver_storage
 * \author	Raphael Lehmann
 */
template <typename BlockDeviceA, typename BlockDeviceB>
class BdMirror : public modm::BlockDevice, protected NestedResumable<3>
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
	static constexpr bd_size_t BlockSizeRead = BlockDeviceA::BlockSizeRead;
	static constexpr bd_size_t BlockSizeWrite = std::max(BlockDeviceA::BlockSizeWrite, BlockDeviceB::BlockSizeWrite);
	static constexpr bd_size_t BlockSizeErase = std::max(BlockDeviceA::BlockSizeErase, BlockDeviceB::BlockSizeErase);
	static constexpr bd_size_t DeviceSize = std::min(BlockDeviceA::DeviceSize, BlockDeviceB::DeviceSize);

public:
	/** Direct access to the BlockDeviceA
	*
	*  @return	BlockDeviceA
	*/
	inline BlockDeviceA& getBlockDeviceA() {return blockDeviceA;};

	/** Direct access to the BlockDeviceB
	*
	*  @return	BlockDeviceB
	*/
	inline BlockDeviceB& getBlockDeviceB() {return blockDeviceB;};

private:
	BlockDeviceA blockDeviceA;
	BlockDeviceB blockDeviceB;

private:
	bool resultA;
	bool resultB;

};

}
#include "block_device_mirror_impl.hpp"

#endif // MODM_BLOCK_DEVICE_MIRROR_HPP
