// coding: utf-8
/* Copyright (c) 2018, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_FILE_HPP
	#error	"Don't include this file directly, use 'block_device_file.hpp' instead!"
#endif
#include "block_device_file.hpp"

// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::initialize()
{
	RF_BEGIN();
	file.open(Filename::name, std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::ate);
	if(!file.is_open())
		RF_RETURN(false);
	if(file.tellg() != DeviceSize) {
		if(file.tellg() == 0) {
			// create empty file with size of DeviceSize
			for(size_t i = 0; i < DeviceSize; i++) {
				file.put(0);
			}
		}
		else {
			RF_RETURN(false);
		}
	}
	RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::deinitialize()
{
	RF_BEGIN();
	file.close();
	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	file.seekg(address);
	file.read(reinterpret_cast<char*>(buffer), size);

	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	file.seekp(address);
	file.write(reinterpret_cast<char*>(const_cast<uint8_t*>(buffer)), size);

	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::erase(bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	// erasing does nothing, memory is undefined after erase and has to be programed first
	RF_END_RETURN(true);
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
modm::ResumableResult<bool>
modm::BdFile<Filename, DeviceSize>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
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
