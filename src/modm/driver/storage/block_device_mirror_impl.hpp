// coding: utf-8
/* Copyright (c) 2018, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BLOCK_DEVICE_MIRROR_HPP
	#error	"Don't include this file directly, use 'block_device_mirror.hpp' instead!"
#endif
#include "block_device_mirror.hpp"


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::initialize()
{
	RF_BEGIN();

	resultA = RF_CALL(blockDeviceA.initialize());
	resultB = RF_CALL(blockDeviceB.initialize());

	RF_END_RETURN(resultA && resultA);
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::deinitialize()
{
	RF_BEGIN();

	resultA = RF_CALL(blockDeviceA.deinitialize());
	resultB = RF_CALL(blockDeviceB.deinitialize());

	RF_END_RETURN(resultA && resultA);
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	return blockDeviceA.read(buffer, address, size);
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeWrite != 0)) {
		RF_RETURN(false);
	}

	resultA = RF_CALL(blockDeviceA.program(buffer, address, size));
	resultB = RF_CALL(blockDeviceB.program(buffer, address, size));

	RF_END_RETURN(resultA && resultA);
}


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::erase(bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0)) {
		RF_RETURN(false);
	}

	resultA = RF_CALL(blockDeviceA.erase(address, size));
	resultB = RF_CALL(blockDeviceB.erase(address, size));

	RF_END_RETURN(resultA && resultA);
}


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
xpcc::ResumableResult<bool>
xpcc::BdMirror<BlockDeviceA, BlockDeviceB>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0)) {
		RF_RETURN(false);
	}

	resultA = RF_CALL(blockDeviceA.write(buffer, address, size));
	resultB = RF_CALL(blockDeviceB.write(buffer, address, size));

	RF_END_RETURN(resultA && resultA);
}
