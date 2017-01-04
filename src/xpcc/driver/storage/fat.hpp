/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FAT_HPP
#define XPCC_FAT_HPP

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
			 * \param command			Command
			 * \param[in,out] buffer	Contains the parameters for the command
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

#endif // XPCC_FAT_HPP
