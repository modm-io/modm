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

#ifndef MODM_BLOCK_DEVICE_HEAP_HPP
	#error	"Don't include this file directly, use 'block_device_heap.hpp' instead!"
#endif
#include <cstring>

// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::initialize()
{
	static_assert(externalMemory == false, "Use modm::BdHeap::initialize(uint8_t* memory) for externalMemory==true");
	std::memset(data, 0, DeviceSize);
	return true;
}

template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::initialize(uint8_t* memory)
{
	static_assert(externalMemory == true, "modm::BdHeap::initialize(uint8_t* memory) is only allowed for externalMemory==true");
	data = memory;
	return true;
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::deinitialize()
{
	return true;
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		return false;
	}

	std::memcpy(buffer, &data[address], size);

	return true;
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	std::memcpy(&data[address], buffer, size);

	return true;
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::erase(bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		return false;
	}

	// erasing does nothing, memory is undefined after erase and has to be programed first
	return true;
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
bool
modm::BdHeap<DeviceSize, externalMemory>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	if(!this->erase(address, size)) {
		return false;
	}

	return this->program(buffer, address, size);
}
