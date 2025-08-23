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

#ifndef MODM_BLOCK_DEVICE_FILE_HPP
	#error	"Don't include this file directly, use 'block_device_file.hpp' instead!"
#endif
#include "block_device_file.hpp"

// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::initialize()
{
	file.open(Filename::name, std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::ate);
	if(!file.is_open())
		return false;
	if(file.tellg() != DeviceSize) {
		if(file.tellg() == 0) {
			// create empty file with size of DeviceSize
			for(size_t i = 0; i < DeviceSize; i++) {
				file.put(0);
			}
		}
		else {
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::deinitialize()
{
	file.close();
	return true;
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		return false;
	}

	file.seekg(address);
	file.read(reinterpret_cast<char*>(buffer), size);

	return true;
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	file.seekp(address);
	file.write(reinterpret_cast<char*>(const_cast<uint8_t*>(buffer)), size);

	return true;
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::erase(bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		return false;
	}

	// erasing does nothing, memory is undefined after erase and has to be programed first
	return true;
}


// ----------------------------------------------------------------------------
template <class Filename, size_t DeviceSize>
bool
modm::BdFile<Filename, DeviceSize>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	if(!this->erase(address, size)) {
		return false;
	}

	return this->program(buffer, address, size);
}
