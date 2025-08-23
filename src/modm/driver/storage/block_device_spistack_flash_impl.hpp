// coding: utf-8
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_SPISTACK_FLASH_HPP
#error	"Don't include this file directly, use 'block_device_spistack_flash.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::initialize()
{
	if (spiBlockDevice.initialize()) {
		spiBlockDevice.selectDie(currentDie = 0x00);
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::deinitialize()
{
	return spiBlockDevice.deinitialize();
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		return false;
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			spiBlockDevice.selectDie(currentDie = dv.quot);
		}
		if (spiBlockDevice.read(&buffer[index], dv.rem, std::min(size - index, DieSize - dv.rem))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			return false;
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			spiBlockDevice.selectDie(currentDie = dv.quot);
		}
		if (spiBlockDevice.program(&buffer[index], dv.rem, std::min(size - index, DieSize - dv.rem))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			return false;
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::erase(bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		return false;
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			spiBlockDevice.selectDie(currentDie = dv.quot);
		}
		if (spiBlockDevice.erase(dv.rem, std::min(size - index, DieSize - dv.rem))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			return false;
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		return false;
	}

	if(!this->erase(address, size)) {
		return false;
	}

	if(!this->program(buffer, address, size)) {
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
bool
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::isBusy()
{
	currentDie = DieCount;
	while (currentDie > 0) {
		spiBlockDevice.selectDie(--currentDie);
		if (spiBlockDevice.isBusy()) {
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
void
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::waitWhileBusy()
{
	while(isBusy()) {
		modm::this_fiber::yield();
	}
}
