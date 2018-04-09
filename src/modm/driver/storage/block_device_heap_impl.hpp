// coding: utf-8
/* Copyright (c) 2017, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_HEAP_HPP
	#error	"Don't include this file directly, use 'block_device_heap.hpp' instead!"
#endif
#include <cstring>

// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::initialize()
{
	static_assert(externalMemory == false, "Use modm::BdHeap::initialize(uint8_t* memory) for externalMemory==true");
	RF_BEGIN();
	std::memset(data, 0, DeviceSize);
	RF_END_RETURN(true);
}

template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::initialize(uint8_t* memory)
{
	static_assert(externalMemory == true, "modm::BdHeap::initialize(uint8_t* memory) is only allowed for externalMemory==true");
	RF_BEGIN();
	data = memory;
	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::deinitialize()
{
	RF_BEGIN();
	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	std::memcpy(buffer, &data[address], size);

	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	std::memcpy(&data[address], buffer, size);

	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::erase(bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	// erasing does nothing, memory is undefined after erase and has to be programed first
	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <size_t DeviceSize, bool externalMemory>
modm::ResumableResult<bool>
modm::BdHeap<DeviceSize, externalMemory>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	if(!RF_CALL(this->erase(address, size))) {
		RF_RETURN(false);
	}

	RF_END_RETURN_CALL(this->program(buffer, address, size));
}
