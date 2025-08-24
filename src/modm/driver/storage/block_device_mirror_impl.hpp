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

#ifndef MODM_BLOCK_DEVICE_MIRROR_HPP
	#error	"Don't include this file directly, use 'block_device_mirror.hpp' instead!"
#endif
#include "block_device_mirror.hpp"


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::initialize()
{
	return blockDeviceA.initialize() and blockDeviceB.initialize();
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::deinitialize()
{
	return blockDeviceA.deinitialize() and blockDeviceB.deinitialize();
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	return blockDeviceA.read(buffer, address, size);
}

// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeWrite != 0)) {
		return false;
	}

	return blockDeviceA.program(buffer, address, size) and blockDeviceB.program(buffer, address, size);
}


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::erase(bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0)) {
		return false;
	}

	return blockDeviceA.erase(address, size) and blockDeviceB.erase(address, size);
}


// ----------------------------------------------------------------------------
template <typename BlockDeviceA, typename BlockDeviceB>
bool
modm::BdMirror<BlockDeviceA, BlockDeviceB>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0)) {
		return false;
	}

	return blockDeviceA.write(buffer, address, size) and blockDeviceB.write(buffer, address, size);
}
