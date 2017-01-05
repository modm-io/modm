/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SPI_RAM_HPP
#define MODM_SPI_RAM_HPP

#include <stdint.h>

namespace modm
{
	/**
	 * \brief	256kBit (64kB) Serial SRAM
	 *
	 * Works with 23K256 (and other).
	 *
	 * - max. 20 MHz SPI
	 * - 2.7-3.6V supply voltage
	 *
	 * \see		<a href="http://ww1.microchip.com/downloads/en/DeviceDoc/22100D.pdf">Datasheet</a>
	 *
	 * \ingroup	driver_storage
	 * \author	Fabian Greif
	 */
	template <typename Spi, typename Cs, typename Hold>
	class SpiRam
	{
	public:
		/**
		 * \brief	Initialize
		 *
		 * Sets used pins as output and switches to continuous operation mode.
		 *
		 * \return	\c true if the device is accessible and the change to
		 * 			continuous mode was successful, \c false otherwise.
		 */
		static bool
		initialize();

		/**
		 * \brief	Start a read transaction
		 *
		 * Make sure to call finish() when all data is written!
		 *
		 * \param	address		15-bit address
		 */
		static void
		startWrite(uint16_t address);

		/// Write one byte
		static void
		writeByte(uint8_t data);

		/// Write a block of data
		static void
		write(const uint8_t *data, std::size_t size);

		/**
		 * \brief	Start a read transaction
		 *
		 * Make sure to call finish() when all data is read!
		 *
		 * \param	address		15-bit address
		 */
		static void
		startRead(uint16_t address);

		/// Read one byte
		static uint8_t
		readByte();

		/// Read a block of data
		static void
		read(uint8_t *data, std::size_t size);

		/**
		 * \brief	Hold communication status
		 *
		 * The device must stay selected during the hold period (CS low). All
		 * pins will go into high impedance state.
		 *
		 * \see		resume()
		 */
		static inline void
		suspend();

		/**
		 * \brief	Resume communication
		 * \see		suspend()
		 */
		static inline void
		resume();

		/**
		 * \brief	Complete transaction
		 *
		 * Must be called after a transaction is completed to deselect the
		 * device and release the bus for other operations.
		 */
		static void
		finish();


		/**
		 * \brief	Convenience function
		 *
		 * Same as:
		 * \code
		 * startWrite(address);
		 * write(data, size);
		 * finish();
		 * \endcode
		 */
		void
		writeData(uint16_t address, const uint8_t *data, std::size_t size);

		/**
		 * \brief	Convenience function
		 *
		 * Same as:
		 * \code
		 * startRead(address);
		 * read(data, size);
		 * finish();
		 * \endcode
		 */
		void
		readData(uint16_t address, uint8_t *data, std::size_t size);

	protected:
		/// Possible SPI instruction codes
		enum Instruction
		{
			READ = 0x03,
			WRITE = 0x02,
			READ_STATUS_REGISTER = 0x05,
			WRITE_STATUS_REGISTER = 0x01
		};

		/// Content of the status register
		enum StatusRegister
		{
			BYTE_MODE = 0x00,
			PAGE_MODE = 0x80,
			SEQUENTIAL_MODE = 0x40,

			HOLD = (1 << 1),	///< 0 = enable hold pin, 1 = disable hold pin
		};

		static Spi spi;
		static Cs cs;
		static Hold hold;
	};
}

#include "spi_ram_impl.hpp"

#endif // MODM_SPI_RAM_HPP
