// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__AT45DB0X1D_HPP
	#error	"Don't include this file directly, use 'at45db0x1d.hpp' instead!"
#endif

#include <xpcc/debug.hpp>

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
xpcc::At45db0x1d<Spi, Cs>::initialize()
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
		Spi::write(0x3d);
		Spi::write(0x2a);
		Spi::write(0x80);
		Spi::write(0xa6);
		
		Cs::set();
	}
	
	waitUntilReady();
	
	return true;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::copyPageToBuffer(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();
	
	if (buffer == at45db::BUFFER_0) {
		Spi::write(MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER);
	}
	else {
		Spi::write(MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER);
	}
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::comparePageToBuffer(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();
	
	if (buffer == at45db::BUFFER_0) {
		Spi::write(MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE);
	}
	else {
		Spi::write(MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE);
	}
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
xpcc::At45db0x1d<Spi, Cs>::isCompareEqual()
{
	return ((readStatus() & COMP) == 0);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::copyBufferToPage(at45db::Buffer buffer, uint16_t pageAddress)
{
	Cs::reset();
	
	if (buffer == at45db::BUFFER_0) {
		Spi::write(BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE);
	}
	else {
		Spi::write(BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE);
	}
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::copyBufferToPageWithoutErase(at45db::Buffer buffer, uint16_t pageAddress)
{
	Cs::reset();
	
	if (buffer == at45db::BUFFER_0) {
		Spi::write(BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE);
	}
	else {
		Spi::write(BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE);
	}
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::readFromBuffer(at45db::Buffer buffer,
		uint8_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	if (buffer == at45db::BUFFER_0) {
		Spi::write(BUFFER_1_READ);
	}
	else {
		Spi::write(BUFFER_2_READ);
	}
	
	// set address
	Spi::write(0);
	Spi::write(0);
	Spi::write(address);
	
	// don't care byte
	Spi::write(0);
	
	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::write(0);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::writeToBuffer(at45db::Buffer buffer,
		uint8_t address, const uint8_t *data, std::size_t size)
{
	Cs::reset();
	if (buffer == at45db::BUFFER_0) {
		Spi::write(BUFFER_1_WRITE);
	}
	else {
		Spi::write(BUFFER_2_WRITE);
	}
	
	// set address
	Spi::write(0);
	Spi::write(0);
	Spi::write(address);
	
	for (std::size_t i = 0; i < size; ++i) {
		Spi::write(*data++);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::readFromMemory(uint32_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	Spi::write(CONTINOUS_ARRAY_READ);
	
	// set address
	Spi::write(address >> 16);
	Spi::write(address >> 8);
	Spi::write(address);
	
	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::write(0);
	}
	Cs::set();
}
		
// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::readPageFromMemory(uint32_t address, uint8_t *data, std::size_t size)
{
	Cs::reset();
	Spi::write(MAIN_MEMORY_PAGE_READ);
	
	// set address
	Spi::write(address >> 16);
	Spi::write(address >> 8);
	Spi::write(address);
	
	// don't care
	Spi::write(0);
	Spi::write(0);
	Spi::write(0);
	Spi::write(0);
	
	for (std::size_t i = 0; i < size; ++i) {
		*data++ = Spi::write(0);
	}
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::pageErase(uint16_t pageAddress)
{
	waitUntilReady();
	
	Cs::reset();
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::pageRewrite(uint16_t pageAddress, at45db::Buffer buffer)
{
	Cs::reset();
	
	if (buffer == at45db::BUFFER_0) {
		Spi::write(BUFFER_1_PAGE_REWRITE);
	}
	else {
		Spi::write(BUFFER_2_PAGE_REWRITE);
	}
	
	// set address
	Spi::write(pageAddress >> 8);
	Spi::write(pageAddress & 0xff);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::blockErase(uint16_t blockAddress)
{
	waitUntilReady();
	
	Cs::reset();
	
	// set address
	Spi::write(blockAddress >> 8);
	Spi::write(blockAddress & 0xf8);
	Spi::write(0);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::chipErase()
{
	waitUntilReady();
	
	Cs::reset();
	
	// send chip erase sequence (fixed sequence)
	Spi::write(0xc7);
	Spi::write(0x94);
	Spi::write(0x80);
	Spi::write(0x9a);
	
	Cs::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
bool
xpcc::At45db0x1d<Spi, Cs>::isReady()
{
	return (readStatus() & READY);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
void
xpcc::At45db0x1d<Spi, Cs>::waitUntilReady()
{
	while (!isReady()) {
		// wait until the device is ready
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs>
uint8_t
xpcc::At45db0x1d<Spi, Cs>::readStatus()
{
	Cs::reset();
	Spi::write(READ_STATUS_REGISTER);
	uint8_t result = Spi::write(0);		// dummy write to get result
	Cs::set();
	
	return result;
}
