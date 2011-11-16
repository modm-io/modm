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

#ifndef XPCC__FAT_HPP
#define XPCC__FAT_HPP

#include <stdint.h>
#include <fatfs/ff.h>
#include <fatfs/diskio.h>

namespace xpcc
{
	namespace fat
	{
		typedef DRESULT Result;
		typedef DSTATUS Status;
		
		/**
		 * \brief	Interface to a SD Card, Dataflash, etc.
		 */
		class PhysicalVolume
		{
		public:
			virtual ~PhysicalVolume()
			{
			}

			/**
			 * \brief	Initialize Volume
			 */
			virtual Status
			initialize() = 0;
			
			/**
			 * \brief	
			 * \return	
			 */
			virtual Status
			getStatus() = 0;
			
			/**
			 * \brief	Read sectors
			 * 
			 * \param buffer		Pointer to the data buffer to store read data
			 * \param sectorNumber	Start sector number (LBA)
			 * \param sectorCount	Sector count (1..255)
			 * 
			 * \return	TODO
			 */
			virtual Result
			read(uint8_t *buffer, int32_t sectorNumber, uint8_t sectorCount) = 0;
			
			/**
			 * \brief	Write Sectors
			 * 
			 * \param buffer		Pointer to the data to be written
			 * \param sectorNumber	Start sector number (LBA)
			 * \param sectorCount	Sector count (1..255)
			 * 
			 * \return	TODO
			 */
			virtual Result
			write(const uint8_t *buffer, int32_t sectorNumber, uint8_t sectorCount) = 0;
			
			/**
			 * \brief	Execute a command
			 * 
			 * \param command	Command
			 * \param[in/out] buffer	Contains the parameters for the command
			 * 							and will be overwritten with the result.
			 */
			virtual Result
			ioctl(uint8_t command, uint32_t *buffer) = 0;
		};
		
		class FileSystem
		{
		public:
			FileSystem(PhysicalVolume *volume, uint8_t drive = 0);
			
			~FileSystem();
			
		protected:
			FATFS fileSystem;
		};
		
		class FileInfo
		{
		public:
			FileInfo();
			
			inline uint32_t
			getSize() const;
			
			inline int16_t
			getModifiedDate() const;
			
			inline int16_t
			getModifiedTime() const;
			
			inline const char*
			getName();
			
			/// Name in 8.3 format
			inline const char*
			getShortName();
			
		protected:
			FILINFO info;
		};
		
		class Directory
		{
		public:
			
		protected:
			DIR directory;
		};
		
		class File
		{
		public:
			void
			open();
			
			void
			close();
			
			void
			read();
			
			void
			lseek();
			
			void
			truncate();
			
			void
			flush();
			
		protected:
			FIL file;
		};
	}
}

#endif // XPCC__FAT_HPP
