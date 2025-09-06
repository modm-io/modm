// coding: utf-8
/*
 * Copyright (c) 2018, Raphael Lehmann
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_DEVICE_SPIFLASH_HPP
#error	"Don't include this file directly, use 'block_device_spiflash.hpp' instead!"
#endif
#include "block_device_spiflash.hpp"

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::initialize()
{
	this->attachConfigurationHandler([]
	{
		Spi::setDataMode(Spi::DataMode::Mode0);
		Spi::setDataOrder(Spi::DataOrder::MsbFirst);
	});
	Cs::setOutput(modm::Gpio::High);

	spiOperation(Instruction::RstEn);
	// Wait T_CPH = 25ns
	spiOperation(Instruction::Rst);
	waitWhileBusy();

	spiOperation(Instruction::WE);
	spiOperation(Instruction::GBU);
	waitWhileBusy();

	// Enter 4-Byte Address mode for serial flash memory that support 256M-bit or more
	if (DeviceSize > ExtendedAddressThreshold) {
		spiOperation(Instruction::En4BAM);
		waitWhileBusy();
	}

	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::deinitialize()
{
	// nothing
	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
typename modm::BdSpiFlash<Spi, Cs, flashSize>::JedecId
modm::BdSpiFlash<Spi, Cs, flashSize>::readId()
{
	spiOperation(Instruction::RJI, nullptr, resultBuffer, 3);

	return JedecId(resultBuffer[0], resultBuffer[1], resultBuffer[2]);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::read(uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeRead != 0) || (address + size > flashSize)) {
		return false;
	}

	waitWhileBusy();
	spiOperation(Instruction::FR, address, nullptr, buffer, size, 1);

	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::program(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeWrite != 0) || (address + size > flashSize)) {
		return false;
	}

	index = 0;
	while(index < size) {
		waitWhileBusy();
		spiOperation(Instruction::WE);
		spiOperation(Instruction::PP, address + index, &buffer[index], nullptr, BlockSizeWrite);
		index += BlockSizeWrite;
	}

	return true;
}


// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::erase(bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (address + size > flashSize)) {
		return false;
	}

	if (address == 0 && size == flashSize) {
		waitWhileBusy();
		spiOperation(Instruction::CE);
	} else {
		index = 0;
		while(index < size) {
			waitWhileBusy();
			spiOperation(Instruction::WE);
			spiOperation(Instruction::SE, address + index);
			index += BlockSizeErase;
		}
	}

	return true;
}


// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::write(const uint8_t* buffer, bd_address_t address, bd_size_t size)
{
	if((size == 0) || (size % BlockSizeErase != 0) || (size % BlockSizeWrite != 0) || (address + size > flashSize)) {
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

// ============================================================================

template <typename Spi, typename Cs, uint32_t flashSize>
typename modm::BdSpiFlash<Spi, Cs, flashSize>::StatusRegister
modm::BdSpiFlash<Spi, Cs, flashSize>::readStatus()
{
	spiOperation(Instruction::RSR1, nullptr, resultBuffer, 1);
	return static_cast<StatusRegister>(resultBuffer[0]);
}

template <typename Spi, typename Cs, uint32_t flashSize>
void
modm::BdSpiFlash<Spi, Cs, flashSize>::selectDie(uint8_t die)
{
	spiOperation(Instruction::SDS, &die, nullptr, 1);
	waitWhileBusy();

	spiOperation(Instruction::WE);
	spiOperation(Instruction::GBU);
	waitWhileBusy();

	// Enter 4-Byte Address mode for serial flash memory that support 256M-bit or more
	if (DeviceSize > ExtendedAddressThreshold) {
		spiOperation(Instruction::En4BAM);
		waitWhileBusy();
	}
}

template <typename Spi, typename Cs, uint32_t flashSize>
bool
modm::BdSpiFlash<Spi, Cs, flashSize>::isBusy()
{
	if(readStatus() & StatusRegister::Busy) {
		return true;
	}

	return false;
}


template <typename Spi, typename Cs, uint32_t flashSize>
void
modm::BdSpiFlash<Spi, Cs, flashSize>::waitWhileBusy()
{
	while(isBusy()) {
		modm::this_fiber::yield();
	}
}

template <typename Spi, typename Cs, uint32_t flashSize>
void
modm::BdSpiFlash<Spi, Cs, flashSize>::spiOperation(Instruction instruction, const uint8_t* tx, uint8_t* rx, std::size_t length, uint8_t nrDummyCycles)
{
	i = 0;
	instructionBuffer[i++] = static_cast<uint8_t>(instruction);
	for(uint8_t j = 0; j < nrDummyCycles; j++) {
		instructionBuffer[i++] = 0x00;
	}

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	Spi::transfer(instructionBuffer, nullptr, i);

	if(length > 0) {
		Spi::transfer(const_cast<uint8_t*>(tx), rx, length);
	}

	if (this->releaseMaster()) {
		Cs::set();
	}
}

template <typename Spi, typename Cs, uint32_t flashSize>
void
modm::BdSpiFlash<Spi, Cs, flashSize>::spiOperation(Instruction instruction, uint32_t address, const uint8_t* tx, uint8_t* rx, std::size_t length, uint8_t nrDummyCycles)
{
	i = 0;
	instructionBuffer[i++] = static_cast<uint8_t>(instruction);
	if constexpr (DeviceSize > ExtendedAddressThreshold) {
		instructionBuffer[i++] = (address >> 24) & 0xFF;
		instructionBuffer[i++] = (address >> 16) & 0xFF;
		instructionBuffer[i++] = (address >> 8) & 0xFF;
		instructionBuffer[i++] = address & 0xFF;
	} else {
		instructionBuffer[i++] = (address >> 16) & 0xFF;
		instructionBuffer[i++] = (address >> 8) & 0xFF;
		instructionBuffer[i++] = address & 0xFF;
	}
	for(uint8_t j = 0; j < nrDummyCycles; j++) {
		instructionBuffer[i++] = 0x00;
	}

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	Spi::transfer(instructionBuffer, nullptr, i);

	if(length > 0) {
		Spi::transfer(const_cast<uint8_t*>(tx), rx, length);
	}

	if (this->releaseMaster()) {
		Cs::set();
	}
}
