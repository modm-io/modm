// coding: utf-8
/* Copyright (c) 2018, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BLOCK_DEVICE_FILE_HPP
#define XPCC_BLOCK_DEVICE_FILE_HPP

#include <xpcc/architecture/interface/block_device.hpp>

#include <xpcc/processing/resumable.hpp>

#include <fstream>
#include <string>

namespace xpcc
{

/**
 * \brief	Block device using a file
 *
 * Persistant and useful for testing and hosted code
 *
 * \ingroup	driver_storage
 * \author	Raphael Lehmann
 */
template <class Filename, size_t DeviceSize>
class BdFile : public xpcc::BlockDevice, protected xpcc::NestedResumable<3>
{
public:
	/// Initializes the storage hardware
	xpcc::ResumableResult<bool>
	initialize();

	/// Deinitializes the storage hardware
	xpcc::ResumableResult<bool>
	deinitialize();

	/** Read data from one or more blocks
	 *
	 *  @param buffer	Buffer to read data into
	 *  @param address	Address to begin reading from
	 *  @param size		Size to read in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	xpcc::ResumableResult<bool>
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
	xpcc::ResumableResult<bool>
	program(const uint8_t* buffer, bd_address_t address, bd_size_t size);

	/** Erase blocks
	 *
	 *  The state of an erased block is undefined until it has been programmed
	 *
	 *  @param address	Address of block to begin erasing
	 *  @param size		Size to erase in bytes (multiple of read block size)
	 *  @return			True on success
	 */
	xpcc::ResumableResult<bool>
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
	xpcc::ResumableResult<bool>
	write(const uint8_t* buffer, bd_address_t address, bd_size_t size);

public:
	static constexpr bd_size_t BlockSizeRead = 1;
	static constexpr bd_size_t BlockSizeWrite = 1;
	static constexpr bd_size_t BlockSizeErase = 1;
private:
	std::fstream file;
};

}
#include "block_device_file_impl.hpp"

#endif // XPCC_BLOCK_DEVICE_FILE_HPP
