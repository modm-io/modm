/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AT45DB0X1D_HPP
	#error	"Don't include this file directly, use 'at45db0x1d.hpp' instead!"
#endif

#include <modm/debug.hpp>

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
modm::At45db0x1d<Spi, Cs>::initialize()
{
	Cs::set();
	Cs::setOutput();

	uint8_t status = readStatus();
	if (status == 0xff || status == 0) {
		// no device present
		return false;
	}

	if ((status & PAGE_SIZE) == 0)
	{
		// Page size is 264 (the default)
		// => set page size from 264 to 256 bytes
		Cs::reset();

		// send page size change sequence (fixed sequence)
		Spi::transfer(0x3d);
		Spi::transfer(0x2a);
		Spi::transfer(0x80);
		Spi::transfer(0xa6);

		Cs::set();
	}

	waitUntilReady();

	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::copyPageToBuffer(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();

	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER);
	}
	else {
		Spi::transfer(MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER);
	}

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::comparePageToBuffer(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();

	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE);
	}
	else {
		Spi::transfer(MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE);
	}

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
modm::At45db0x1d<Spi, Cs>::isCompareEqual()
{
	return ((readStatus() & COMP) == 0);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::copyBufferToPage(at45db::Buffer buffer, uint16_t pageAddress)
{
	Cs::reset();

	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE);
	}
	else {
		Spi::transfer(BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE);
	}

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::copyBufferToPageWithoutErase(at45db::Buffer buffer, uint16_t pageAddress)
{
	Cs::reset();

	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE);
	}
	else {
		Spi::transfer(BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE);
	}

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::readFromBuffer(at45db::Buffer buffer,
		uint8_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(BUFFER_1_READ);
	}
	else {
		Spi::transfer(BUFFER_2_READ);
	}

	// set address
	Spi::transfer(0);
	Spi::transfer(0);
	Spi::transfer(address);

	// don't care byte
	Spi::transfer(0);

	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::transfer(0);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::writeToBuffer(at45db::Buffer buffer,
		uint8_t address, const uint8_t *data, std::size_t size)
{
	Cs::reset();
	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(BUFFER_1_WRITE);
	}
	else {
		Spi::transfer(BUFFER_2_WRITE);
	}

	// set address
	Spi::transfer(0);
	Spi::transfer(0);
	Spi::transfer(address);

	for (std::size_t i = 0; i < size; ++i) {
		Spi::transfer(*data++);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::readFromMemory(uint32_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	Spi::transfer(CONTINOUS_ARRAY_READ);

	// set address
	Spi::transfer(address >> 16);
	Spi::transfer(address >> 8);
	Spi::transfer(address);

	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::transfer(0);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::readPageFromMemory(uint32_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	Spi::transfer(MAIN_MEMORY_PAGE_READ);

	// set address
	Spi::transfer(address >> 16);
	Spi::transfer(address >> 8);
	Spi::transfer(address);

	// don't care
	Spi::transfer(0);
	Spi::transfer(0);
	Spi::transfer(0);
	Spi::transfer(0);

	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::transfer(0);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::pageErase(uint16_t pageAddress)
{
	waitUntilReady();

	Cs::reset();

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::pageRewrite(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();

	if (buffer == at45db::BUFFER_0) {
		Spi::transfer(BUFFER_1_PAGE_REWRITE);
	}
	else {
		Spi::transfer(BUFFER_2_PAGE_REWRITE);
	}

	// set address
	Spi::transfer(pageAddress >> 8);
	Spi::transfer(pageAddress & 0xff);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::blockErase(uint16_t blockAddress)
{
	waitUntilReady();

	Cs::reset();

	// set address
	Spi::transfer(blockAddress >> 8);
	Spi::transfer(blockAddress & 0xf8);
	Spi::transfer(0);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::chipErase()
{
	waitUntilReady();

	Cs::reset();

	// send chip erase sequence (fixed sequence)
	Spi::transfer(0xc7);
	Spi::transfer(0x94);
	Spi::transfer(0x80);
	Spi::transfer(0x9a);

	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
modm::At45db0x1d<Spi, Cs>::isReady()
{
	return (readStatus() & READY);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
modm::At45db0x1d<Spi, Cs>::waitUntilReady()
{
	while (!isReady()) {
		// wait until the device is ready
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
uint8_t
modm::At45db0x1d<Spi, Cs>::readStatus()
{
	Cs::reset();
	Spi::transfer(READ_STATUS_REGISTER);
	uint8_t result = Spi::transfer(0);		// dummy write to get result
	Cs::set();

	return result;
}
