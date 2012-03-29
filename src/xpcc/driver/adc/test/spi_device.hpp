// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
				std::size_t transmissionCount);
		
		/**
		 * Check if all transmission were finished successful (content &
		 * size correct).
		 */
		bool
		isSuccessful();
		
		/// Dump an Error report to the console
		void
		reportErrors();
		
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
		
		bool successful;
		bool complete;
	};
}

#endif // TEST__SPI_DEVICE_HPP

