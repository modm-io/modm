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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__AT45DB0X1D_HPP
#define XPCC__AT45DB0X1D_HPP

namespace xpcc
{
	/**
	 * \see		At45db0x1d
	 * \ingroup	memory
	 */
	namespace at45db
	{
		enum Buffer
		{
			BUFFER_0 = 0,
			BUFFER_1 = 1
		};
	}
	
	/**
	 * \brief	Atmel DataFlash
	 * 
	 * Works with:
	 * AT45DB011D (128kB), AT45DB021D (256kB), AT45DB041D (512kB) and AT45DB081D (1MB)
	 * 
	 * Features:
	 * - 2.7 to 3.6V Supply Voltage
	 * - 66Mhz SPI interface
	 * - 256 byte page size
	 * 
	 * \code
	 * Operation                  | Time
	 * ---------------------------+--------
	 * Page to buffer             |   200µs
	 * Page to buffer compare     |   200µs
	 * Page erase and programming | 14-35ms
	 * Page programming time      |   2-4ms
	 * Page erase time            | 13-32ms
	 * Block erase time           | 30-75ms
	 * Sector erase time          | 1.6-5s
	 * \endcode
	 * 
	 * \see		at45db
	 * \see		<a href="http://www.atmel.com/dyn/resources/prod_documents/doc3595.pdf">Datasheet</a>
	 * 
	 * \ingroup	memory
	 * \author	Fabian Greif
	 */
	template <typename Spi, typename Cs>
	class At45db0x1d
	{
	public:
		/**
		 * \brief	Initialize
		 * 
		 * Sets used pins as outputs and switches to binary page size mode
		 * (if not already active).
		 * 
		 * \return	\c true if the device is present, \c false if no
		 * 			device was found
		 */
		static bool
		initialize();
		
		/**
		 * \brief	Copy memory page to a buffer
		 * 
		 * Takes ~200µs. Operation is finished when isReady() returns \c true.
		 * 
		 * \param	pageAddress	Page address (upper 11-bits of the 19-bit address)
		 * \param	buffer		Buffer index (0..1)
		 * 
		 * \see		isReady()
		 */
		static void
		copyPageToBuffer(uint16_t pageAddress, at45db::Buffer buffer);
		
		/**
		 * \brief	Check if the content of a buffer matches the content of
		 * 			a memory page
		 * 
		 * Use isEqual() to check the result.
		 * 
		 * \param	pageAddress	Page address (upper 11-bits of the 19-bit address)
		 * \param	buffer		Buffer index (0..1)
		 * 
		 * \see		isReady()
		 * \see		isEqual()
		 */
		static void
		comparePageToBuffer(uint16_t pageAddress, at45db::Buffer buffer);
		
		/**
		 * \brief	Check the result of a compare operation
		 * 
		 * Output is only valid after a comparePageToBuffer() operation and
		 * when isReady() returns \c true! Any other time the result is
		 * undefined.
		 * 
		 * \return	\c true if the content during the last compare operation
		 * 			matches, \c false on any difference.
		 */
		static bool
		isEqual();
		
		/**
		 * \brief	Write to buffer to a memory page
		 * 
		 * Takes 14-35ms. Operation is finished when isReady() returns \c true.
		 * 
		 * \param	buffer		Buffer index (0..1)
		 * \param	pageAddress	Page address (upper 11-bits of the 19-bit address)
		 * 
		 * \see		isReady()
		 */
		static void
		copyBufferToPage(at45db::Buffer buffer, uint16_t pageAddress);
		
		/**
		 * \brief	Write to buffer to a memory page without erasing the page
		 * 
		 * This operation is 2-3ms faster than copyBufferToPage() and requires
		 * the content of the page to be empty (all 0xff). Despite that it is
		 * equal to copyBufferToPage().
		 * 
		 * \see		copyBufferToPage()
		 */
		static void
		copyBufferToPageWithoutErase(at45db::Buffer buffer, uint16_t pageAddress);
		
		
		/**
		 * \brief	Read data from a buffer
		 * 
		 * When the end of the buffer is reached the address pointer wraps
		 * around to the beginning of the buffer.
		 * 
		 * \param buffer	Buffer index (0..1)
		 * \param address	Address within the buffer
		 * \param[out] data	Target buffer
		 * \param size		Number of bytes to read
		 */
		static void
		readFromBuffer(at45db::Buffer buffer, uint8_t address, uint8_t *data, uint8_t size);
		
		/**
		 * \brief	Write data to a buffer
		 * 
		 * When the end of the buffer is reached the address pointer wraps
		 * around to the beginning of the buffer.
		 * 
		 * \param buffer	Buffer index (0..1)
		 * \param address	Address within the buffer
		 * \param[in] data	Target buffer	
		 * \param size		Number of bytes to write
		 */
		static void
		writeToBuffer(at45db::Buffer buffer, uint8_t address, const uint8_t *data, uint8_t size);
		
		/**
		 * \brief	Continuous read from the memory
		 * 
		 * \param address	19-bit address
		 * \param[out] data	Target buffer
		 * \param size		Number of bytes to read
		 */
		static void
		readFromMemory(uint32_t address, uint8_t *data, std::size_t size);
		
		/**
		 * \brief	Read a page from the memory (bypassing the buffers)
		 * 
		 * When the end of the page is reached the address pointer wraps
		 * around to the beginning of the page.
		 * 
		 * \param address	19-bit address (upper 11-bit specify the page,
		 * 					lower 8-bit specify the position inside the page)
		 * \param[out] data	Target buffer
		 * \param size		Number of bytes to read
		 */
		static void
		readPageFromMemory(uint32_t address, uint8_t *data, std::size_t size);
		
		/**
		 * \brief	Erase page
		 * 
		 * Takes 13-32ms. Operation is finished when isReady() returns \c true.
		 * 
		 * \param	pageAddress	Page address (upper 11-bits of the 19-bit address)
		 * \see		isReady()
		 */
		static void
		pageErase(uint16_t pageAddress);
		
		/**
		 * \brief	Rewrite page
		 * 
		 * Takes 14-35ms. Operation is finished when isReady() returns \c true.
		 * 
		 * This operation is a combination of two operations: Main Memory Page
		 * to Buffer Transfer and Buffer to Main Memory Page Program with
		 * Built-in Erase. A page of data is first transferred from the
		 * main memory to buffer 0 or 1, and then the same data (from
		 * buffer 0 or 1) is programmed back into its original page of
		 * main memory.
		 * 
		 * Each page within a sector must be updated/rewritten at least once
		 * within every 10,000 cumulative page erase/program operations in
		 * that sector.
		 * 
		 * \param	pageAddress	Page address (upper 11-bits of the 19-bit address)
		 * \param	buffer		Buffer index (0..1)
		 * 
		 * \see		isReady()
		 */
		static void
		pageRewrite(uint16_t pageAddress, at45db::Buffer buffer);
		
		/**
		 * \brief	Erase a block of eight pages
		 * 
		 * Takes 30-75ms. Operation is finished when isReady() returns \c true.
		 * 
		 * \param 	blockAddress	Block address (upper 11-bits of the 19-bit
		 * 					address, from these 11-bits the three lower bits
		 * 					are don't care).
		 * \see		isReady()
		 */
		static void
		blockErase(uint16_t blockAddress);
		
		/**
		 * \brief	Erase entire chip
		 * 
		 * This will take some seconds. Operation is finished when isReady()
		 * returns \c true.
		 * 
		 * \see		isReady()
		 */
		static void
		chipErase();
		
		
		
		/// Check if the device is ready for the next operation
		static inline bool
		isReady();
		
		/// Wait until isReady() returns \c true
		static void
		waitUntilReady();
		
	protected:
		enum Opcode
		{
			CONTINOUS_ARRAY_READ_HIGH_FREQ = 0x0b,	///< high frequency (up to 66 Mhz)
			CONTINOUS_ARRAY_READ = 0x03,		///< low frequency (up to 33 MHz)
			MAIN_MEMORY_PAGE_READ = 0xd2,		///< read directly, bypassing the buffers
			BUFFER_1_READ = 0xd4,				///< read from buffer 1
			BUFFER_2_READ = 0xd6,				///< read from buffer 2
			BUFFER_1_WRITE = 0x84,				///< write to buffer 1
			BUFFER_2_WRITE = 0x87,				///< write to buffer 2
			MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER = 0x53,	///< transfer data from the main memory to buffer 1
			MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER = 0x55,	///< transfer data from the main memory to buffer 2
			MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE = 0x60,	///< compare content of the main memory with the content of buffer 1
			MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE = 0x61,	///< compare content of the main memory with the content of buffer 2
			BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE = 0x83,
			BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITH_ERASE = 0x86,
			BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE = 0x88,
			BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_WITHOUT_ERASE = 0x89,
			BUFFER_1_PAGE_REWRITE = 0x58,
			BUFFER_2_PAGE_REWRITE = 0x59,			
			MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_1 = 0x82,
			MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_2 = 0x85,
			PAGE_ERASE = 0x81,
			BLOCK_ERASE = 0x50,					///< erase a block of eight pages
			SECTOR_ERASE = 0x7c,
			READ_STATUS_REGISTER = 0xd7,
			DEEP_POWER_DOWN = 0xb9,
			RESUME_FROM_POWER_DOWN = 0xab,
		};
		
		enum StatusRegister
		{
			READY = (1 << 7),	// 0 = busy, 1 = ready
			COMP = (1 << 6),	// 0 = data from memory matches the buffer
			PROTECT = (1 << 1),	// 1 = sector protection enabled
			PAGE_SIZE = (1 << 0),	// 0 = 264 byte, 1 = 256 byte pagesize
		};
		
		static uint8_t
		readStatus(void);
		
		static Spi spi;
		static Cs cs;
	};
}

#include "at45db0x1d_impl.hpp"

#endif // XPCC__AT45DB0X1D_HPP
