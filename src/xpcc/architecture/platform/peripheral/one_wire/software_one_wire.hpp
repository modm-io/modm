// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef XPCC__SOFTWARE_ONE_WIRE_HPP
#define XPCC__SOFTWARE_ONE_WIRE_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Software emulation of a 1-wire master
	 * 
	 * 1-Wire is extremely timing critical. This implementation relies
	 * on simple delay loops to achieve this timing. Any interrupt during
	 * the operation can disturb the timing.
	 * 
	 * You should make sure that no interrupt occurs during the 1-Wire
	 * transmissions, for example by disabling interrupts.
	 * 
	 * Based on the Maxim 1-Wire AppNote at
	 * http://www.maxim-ic.com/appnotes.cfm/appnote_number/126
	 *
	 * 1-Wire Search Algorithm based on AppNote 187 at
	 * http://www.maxim-ic.com/appnotes.cfm/appnote_number/187
	 * 
	 * \ingroup	1-wire
	 */
	template <typename Pin>
	class SoftwareOneWire
	{
	public:
		/**
		 * \brief	Initialize the 1-wire bus
		 */
		static void
		initialize();
		
		/**
		 * \brief	Generate a 1-wire reset
		 * 
		 * \return	\c true devices detected, \n
		 * 			\c false failed to detect devices
		 */
		static bool
		touchReset();
		
		/**
		 * \brief	Send a 1-wire write bit.
		 * 
		 * Provides 10us recovery time.
		 */
		static void
		writeBit(bool bit);
		
		/**
		 * \brief	Read a bit from the 1-wire bus and return it.
		 * 
		 * Provides 10us recovery time.
		 */
		static bool
		readBit();
		
		/// Write 1-Wire data byte
		static void
		writeByte(uint8_t data);
		
		/// Read 1-Wire data byte and return it
		static uint8_t
		readByte();
		
		/// Write a 1-Wire data byte and return the sampled result.
		static uint8_t
		touchByte(uint8_t data);
		
		/**
		 * \brief	Verify that the with the given ROM number is present
		 * 
		 * \param 	rom		8-byte ROM number 
		 * \return	\c true device presens verified, \n
		 * 			\c false device not present 
		 */
		static bool
		verifyDevice(const uint8_t *rom);
		
		/**
		 * \brief	Reset search state
		 * \see		searchNext()
		 */
		static void
		resetSearch();
		
		/**
		 * \brief	Reset search state and setup it to find the device type
		 * 			'familyCode' on the next call to searchNext().
		 * 
		 * This will accelerate the search because only devices of the given
		 * type will be considered.
		 */
		static void
		resetSearch(uint8_t familyCode);
		
		/**
		 * \brief	Perform the 1-Wire search algorithm on the 1-Wire bus
		 * 			using the existing search state.
		 * 
		 * \param[out]	rom		8 byte array which will be filled with ROM
		 * 						number of the device found.
		 * \return	\c true is a device is found. \p rom will contain the
		 * 			ROM number. \c false if no device found. This also
		 * 			marks the end of the search.
		 * 
		 * \see		resetSearch()
		 */
		static bool
		searchNext(uint8_t *rom);
		
		/**
		 * \brief	Setup the search to skip the current device type on the
		 * 			next call to searchNext()
		 */
		static void
		searchSkipCurrentFamily();
		
	protected:
		/**
		 * \internal
		 * \brief	Universal base class for the SAB interface
		 * 
		 * \see		<a href="http://www.maxim-ic.com/app-notes/index.mvp/id/27" target="_blank">
		 * 			Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products</a>
		 * \ingroup	sab
		 */
		static uint8_t
		crcUpdate(uint8_t crc, uint8_t data);
		
		/// Perform the actual search algorithm
		static bool
		performSearch();
		
		// standard delay times in µs
		static const int A = 6;
		static const int B = 64;
		static const int C = 60;
		static const int D = 10;
		static const int E = 9;
		static const int F = 55;
		static const int G = 0;
		static const int H = 480;
		static const int I = 70;
		static const int J = 410;
		
		static Pin pin;
		
		// state of the search
		static uint8_t lastDiscrepancy;
		static uint8_t lastFamilyDiscrepancy;
		static bool lastDeviceFlag;
		static uint8_t crc8;
		static uint8_t romBuffer[8];
	};
}

#include "software_one_wire_impl.hpp"

#endif // XPCC__SOFTWARE_ONE_WIRE_HPP
