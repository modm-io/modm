/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SOFTWARE_ONE_WIRE_HPP
#define MODM_SOFTWARE_ONE_WIRE_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/driver/delay.hpp>

namespace modm
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
	class SoftwareOneWireMaster
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
		
		// standard delay times in microseconds
		static constexpr unsigned int A = 6;
		static constexpr unsigned int B = 64;
		static constexpr unsigned int C = 60;
		static constexpr unsigned int D = 10;
		static constexpr unsigned int E = 9;
		static constexpr unsigned int F = 55;
		static constexpr unsigned int G = 0;
		static constexpr unsigned int H = 480;
		static constexpr unsigned int I = 70;
		static constexpr unsigned int J = 410;
		
		static Pin pin;
		
		// state of the search
		static uint8_t lastDiscrepancy;
		static uint8_t lastFamilyDiscrepancy;
		static bool lastDeviceFlag;
		static uint8_t crc8;
		static uint8_t romBuffer[8];
	};
}

#include "one_wire_master_impl.hpp"

#endif // MODM_SOFTWARE_ONE_WIRE_HPP
