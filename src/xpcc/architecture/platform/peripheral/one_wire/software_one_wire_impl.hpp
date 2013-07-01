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
	#error	"Don't include this file directly, use 'software_one_wire.hpp' instead!"
#endif

#ifdef __AVR__
#	include <util/crc16.h>
#endif

// ----------------------------------------------------------------------------
template <typename Pin> Pin xpcc::SoftwareOneWire<Pin>::pin;

template <typename Pin> uint8_t xpcc::SoftwareOneWire<Pin>::lastDiscrepancy;
template <typename Pin> uint8_t xpcc::SoftwareOneWire<Pin>::lastFamilyDiscrepancy;
template <typename Pin> bool xpcc::SoftwareOneWire<Pin>::lastDeviceFlag;
template <typename Pin> uint8_t xpcc::SoftwareOneWire<Pin>::crc8;
template <typename Pin> uint8_t xpcc::SoftwareOneWire<Pin>::romBuffer[8];

// ----------------------------------------------------------------------------
template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::initialize()
{
	pin.setInput();
	pin.reset();
}

template <typename Pin>
bool
xpcc::SoftwareOneWire<Pin>::touchReset()
{
	delay_us(G);
	pin.setOutput();	// drives the bus low
	delay_us(H);
	pin.setInput();		// releases the bus
	delay_us(I);
	
	// sample for presence pulse from slave
	bool result = !pin.read();
	
	delay_us(J);			// complete the reset sequence recovery
	
	return result;
}

template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::writeBit(bool bit)
{
	if (bit)
	{
		pin.setOutput();	// Drives bus low
		delay_us(A);
		pin.setInput();		// Releases the bus
		delay_us(B);		// Complete the time slot and 10us recovery
	}
	else
	{
		pin.setOutput();
		delay_us(C);
		pin.setInput();
		delay_us(D);
	}
}

template <typename Pin>
bool
xpcc::SoftwareOneWire<Pin>::readBit()
{
	pin.setOutput();	// drives the bus low
	delay_us(A);
	pin.setInput();		// releases the bus
	delay_us(E);
	
	// Sample the bit value from the slave
	bool result = pin.read();
	
	delay_us(F);			// complete the reset sequence recovery
	
	return result;
}

// ----------------------------------------------------------------------------
template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::writeByte(uint8_t data)
{
	// Loop to write each bit in the byte, LS-bit first
	for (uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x01);
		data >>= 1;
	}
}

template <typename Pin>
uint8_t
xpcc::SoftwareOneWire<Pin>::readByte()
{
	uint8_t result = 0;
	for (uint8_t i = 0; i < 8; ++i)
	{
		result >>= 1;
		if (readBit()) {
			result |= 0x80;
		}
	}
	
	return result;
}

// ----------------------------------------------------------------------------
template <typename Pin>
uint8_t
xpcc::SoftwareOneWire<Pin>::touchByte(uint8_t data)
{
	uint8_t result = 0;
	for (uint8_t i = 0; i < 8; ++i)
	{
		result >>= 1;
		
		// If sending a '1' then read a bit else write a '0'
		if (data & 0x01)
		{
			if (readBit()) {
				result |= 0x80;
			}
		}
		else {
			writeBit(0);
		}
		
		data >>= 1;
	}
	
	return result;
}

// ----------------------------------------------------------------------------
template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::resetSearch()
{
	// reset the search state
	lastDiscrepancy = 0;
	lastFamilyDiscrepancy = 0;
	lastDeviceFlag = false;
}

template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::resetSearch(uint8_t familyCode)
{
	// set the search state to find family type devices
	romBuffer[0] = familyCode;
	for (uint8_t i = 1; i < 8; ++i) {
		romBuffer[i] = 0;
	}
	
	lastDiscrepancy = 64;
	lastFamilyDiscrepancy = 0;
	lastDeviceFlag = false;
}

template <typename Pin>
bool
xpcc::SoftwareOneWire<Pin>::searchNext(uint8_t *rom)
{
	if (performSearch())
	{
		for (uint8_t i = 0; i < 8; ++i) {
			rom[i] = romBuffer[i];
		}
		return true;
	}
	return false;
}

template <typename Pin>
void
xpcc::SoftwareOneWire<Pin>::searchSkipCurrentFamily()
{
	// set the Last discrepancy to last family discrepancy
	lastDiscrepancy = lastFamilyDiscrepancy;
	lastFamilyDiscrepancy = 0;
	
	// check for end of list
	if (lastDiscrepancy == 0) {
		lastDeviceFlag = true;
	}
}

template <typename Pin>
bool
xpcc::SoftwareOneWire<Pin>::verifyDevice(const uint8_t *rom)
{
	uint8_t romBufferBackup[8];
	bool result;
	
	// keep a backup copy of the current state
	for (uint8_t i = 0; i < 8; i++) {
		romBufferBackup[i] = romBuffer[i];
	}
	uint16_t ld_backup = lastDiscrepancy;
	bool ldf_backup = lastDeviceFlag;
	uint16_t lfd_backup = lastFamilyDiscrepancy;
	
	
	// set search to find the same device
	lastDiscrepancy = 64;
	lastDeviceFlag = false;
	if (performSearch())
	{
		// check if same device found
		result = true;
		for (uint8_t i = 0; i < 8; i++)
		{
			if (rom[i] != romBuffer[i])
			{
				result = false;
				break;
			}
		}
	}
	else {
		result = false;
	}
	
	// restore the search state
	for (uint8_t i = 0; i < 8; i++) {
		romBuffer[i] = romBufferBackup[i];
	}
	lastDiscrepancy = ld_backup;
	lastDeviceFlag = ldf_backup;
	lastFamilyDiscrepancy = lfd_backup;
	
	// return the result of the verify
	return result;
}

// ----------------------------------------------------------------------------
template <typename Pin>
uint8_t
xpcc::SoftwareOneWire<Pin>::crcUpdate(uint8_t crc, uint8_t data)
{
#ifdef __AVR__
	return _crc_ibutton_update(crc, data);
#else
	crc = crc ^ data;
	for (uint_fast8_t i = 0; i < 8; ++i)
	{
		if (crc & 0x01) {
			crc = (crc >> 1) ^ 0x8C;
		}
		else {
			crc >>= 1;
		}
	}
	return crc;
#endif
}

// ----------------------------------------------------------------------------
template <typename Pin>
bool
xpcc::SoftwareOneWire<Pin>::performSearch()
{
	bool searchResult = false;
	
	// if the last call was not the last one
	if (!lastDeviceFlag)
	{
		// 1-Wire reset
		if (!touchReset())
		{
			// reset the search
			lastDiscrepancy = 0;
			lastDeviceFlag = false;
			lastFamilyDiscrepancy = 0;
			return false;
		}
		
		// issue the search command
		writeByte(0xF0);
		
		// initialize for search
		uint8_t idBitNumber = 1;
		uint8_t lastZeroBit = 0;
		uint8_t romByteNumber = 0;
		uint8_t romByteMask = 1;
		bool searchDirection;
		
		crc8 = 0;
		
		// loop to do the search
		do
		{
			// read a bit and its complement
			bool idBit = readBit();
			bool complementIdBit = readBit();
			
			// check for no devices on 1-wire
			if ((idBit == true) && (complementIdBit == true)) {
				break;
			}
			else
			{
				// all devices coupled have 0 or 1
				if (idBit != complementIdBit) {
					searchDirection = idBit; // bit write value for search
				}
				else
				{
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (idBitNumber < lastDiscrepancy) {
						searchDirection = ((romBuffer[romByteNumber] & romByteMask) > 0);
					}
					else {
						// if equal to last pick 1, if not then pick 0
						searchDirection = (idBitNumber == lastDiscrepancy);
					}
					
					// if 0 was picked then record its position in LastZero
					if (searchDirection == false)
					{
						lastZeroBit = idBitNumber;
						// check for Last discrepancy in family
						if (lastZeroBit < 9)
							lastFamilyDiscrepancy = lastZeroBit;
					}
				}
				
				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (searchDirection == true) {
					romBuffer[romByteNumber] |= romByteMask;
				}
				else {
					romBuffer[romByteNumber] &= ~romByteMask;
				}
				
				// serial number search direction write bit
				writeBit(searchDirection);
				
				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				idBitNumber++;
				romByteMask <<= 1;
				
				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (romByteMask == 0)
				{
					crc8 = crcUpdate(crc8, romBuffer[romByteNumber]); // accumulate the CRC
					romByteNumber++;
					romByteMask = 1;
				}
			}
		}
		while(romByteNumber < 8);  // loop until through all ROM bytes 0-7

		// if the search was successful then
		if (!((idBitNumber < 65) || (crc8 != 0)))
		{
			// search successful
			lastDiscrepancy = lastZeroBit;
			// check for last device
			if (lastDiscrepancy == 0) {
				lastDeviceFlag = true;
			}
			searchResult = true;
		}
	}
	
	// if no device found then reset counters so next 'search' will be like a first
	if (!searchResult || !romBuffer[0])
	{
		lastDiscrepancy = 0;
		lastDeviceFlag = false;
		lastFamilyDiscrepancy = 0;
		searchResult = false;
	}
	
	return searchResult;
}
