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
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::initialize()
{
	RF_BEGIN();

	if (RF_CALL(spiBlockDevice.initialize())) {
		RF_CALL(spiBlockDevice.selectDie(currentDie = 0x00));
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::deinitialize()
{
	RF_BEGIN();
	RF_END_RETURN_CALL(spiBlockDevice.deinitialize());
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			RF_CALL(spiBlockDevice.selectDie(currentDie = dv.quot));
		}
		if (RF_CALL(spiBlockDevice.read(&buffer[index], dv.rem, std::min(size - index, DieSize - dv.rem)))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			RF_RETURN(false);
		}
	}

	RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			RF_CALL(spiBlockDevice.selectDie(currentDie = dv.quot));
		}
		if (RF_CALL(spiBlockDevice.program(&buffer[index], dv.rem, std::min(size - index, DieSize - dv.rem)))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			RF_RETURN(false);
		}
	}

	RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::erase(bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	index = 0;
	while (index < size) {
		dv = std::ldiv(index + address, DieSize); // dv.quot = die #ID, dv.rem = die address
		if (currentDie != dv.quot) {
			RF_CALL(spiBlockDevice.selectDie(currentDie = dv.quot));
		}
		if (RF_CALL(spiBlockDevice.erase(dv.rem, std::min(size - index, DieSize - dv.rem)))) {
			index += DieSize - dv.rem; // size - index <= DieSize - dv.rem only on last iteration!
		} else {
			RF_RETURN(false);
		}
	}

	RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	RF_BEGIN();

	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > DeviceSize)) {
		RF_RETURN(false);
	}

	if(!RF_CALL(this->erase(address, size))) {
		RF_RETURN(false);
	}

	if(!RF_CALL(this->program(buffer, address, size))) {
		RF_RETURN(false);
	}

	RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<bool>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::isBusy()
{
	RF_BEGIN();

	currentDie = DieCount;
	while (currentDie > 0) {
		RF_CALL(spiBlockDevice.selectDie(--currentDie));
		if (RF_CALL(spiBlockDevice.isBusy())) {
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------

template <typename SpiBlockDevice, uint8_t DieCount>
modm::ResumableResult<void>
modm::BdSpiStackFlash<SpiBlockDevice, DieCount>::waitWhileBusy()
{
	RF_BEGIN();
	while(RF_CALL(isBusy())) {
		RF_YIELD();
	}
	RF_END();
}
