/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TEST__SPI_DEVICE_HPP
#define TEST__SPI_DEVICE_HPP

#include <cstddef>
#include <stdint.h>

namespace test
{
	#ifdef ENABLE_MACRO_EXPORT
	#	define ANONYMOUS_ARRAY(type, ...)	((type []) { __VA_ARGS__ })
	#	define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))	
	#	define RX_DATA(...)		ANONYMOUS_ARRAY(uint8_t, __VA_ARGS__)
	#	define TX_DATA(...)		ANONYMOUS_ARRAY(uint8_t, __VA_ARGS__)
	#endif
	
	/**
	 * Single SPI transmission
	 * 
	 * Example:
	 * \code
	 * 
	 * \endcode
	 */
	struct Transmission
	{
		Transmission(std::size_t length, uint8_t* rx, uint8_t* tx) :
			length(length), rx(rx), tx(tx)
		{
		}
		
		std::size_t length;
		
		uint8_t *rx;
		uint8_t *tx;
	};
	
	/**
	 * Mock-Up class to simulate SPI devices.
	 * 
	 * It has a separate start() function instead of using the Constructor
	 * to allow the usage with static classes.
	 * 
	 * \author	Fabian Greif
	 */
	class SpiDevice
	{
	public:
		SpiDevice();
		
		~SpiDevice();
		
		void
		select();
		
		void
		deselect();
		
		uint8_t
		write(uint8_t data);
		
		/**
		 * Start a new set of transmissions.
		 * 
		 * Sets the expected transmissions and resets the internal state.
		 */
		void
		start(const Transmission* transmissions,
				std::size_t transmissionCount,
				std::size_t line,
				bool reportErrors = true);
		
		/**
		 * Check if all transmission were finished successful (content &
		 * size correct).
		 */
		void
		finish();
		
		bool
		isSuccessful()
		{
			return (complete && (error == NO_ERROR));
		}
		
	private:
		/// Transmission data
		const Transmission* transmissions;
		
		/// Total number of expected transmissions
		std::size_t transmissionCount;
		
		std::size_t currentTransmission;
		
		/// Number of Bytes written during the current transmission
		std::size_t bytesWritten;
		
		/// Device is selected via the CS pin
		bool selected;
		
		bool complete;
		
		enum Error
		{
			NO_ERROR = 0,
			DOUBLE_SELECT = (1 << 1),
			MAX_TRANSMISSION_COUNT_EXCEEDED = (1 << 2),		///< more transmissions than expected
			WRONG_LENGTH = (1 << 3),		///< Transmission is to short or to long
			WRONG_DATA_RECEIVED = (1 << 4),			///< Other data bytes received
		};
		uint8_t error;
		
		std::size_t lineNumber;
		bool reportErrors;
		uint8_t *rxBuffer;
	};
}

#endif // TEST__SPI_DEVICE_HPP

