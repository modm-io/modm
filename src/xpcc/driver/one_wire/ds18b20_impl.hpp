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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__DS18B20_HPP
	#error	"Don't include this file directly, use 'ds18b20.hpp' instead!"
#endif

#include <cstring>		// for std::memcpy

template <typename OneWire>
xpcc::DS18B20<OneWire>::DS18B20(const uint8_t *rom)
{
	std::memcpy(this->identifier, rom, 8);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
xpcc::DS18B20<OneWire>::isAvailable()
{
	return ow.verifyDevice(this->identifier);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
void
xpcc::DS18B20<OneWire>::startConversion()
{
	selectDevice();
	
	ow.writeByte(CONVERT_T);
}

template <typename OneWire>
void
xpcc::DS18B20<OneWire>::startConversions()
{
	//Reset the bus / Initialization
	if (!ow.touchReset()) {
		//no devices detected
		return;
	}

	// Send this to everybody
	ow.writeByte(one_wire::SKIP_ROM);
	
	// Issue Convert Temperature command
	ow.writeByte(this->CONVERT_T);
}

template <typename OneWire>
bool
xpcc::DS18B20<OneWire>::isConversionDone()
{
	return ow.readBit();
}

// ----------------------------------------------------------------------------
template <typename OneWire>
int16_t
xpcc::DS18B20<OneWire>::readTemperature()
{
	selectDevice();
	ow.writeByte(this->READ_SCRATCHPAD);
	
	// Read the first bytes of the scratchpad memory
	// and then send a reset because we do not want the other bytes
	
	int16_t temp = ow.readByte();
	temp |= (ow.readByte() << 8);
	
	// ignore next two bytes
	ow.readByte();
	ow.readByte();
	
	// read config register byte
	uint8_t config = (ow.readByte() >> 5) & 0x03;
	
	ow.touchReset();
	
	// Calculate conversion factor depending on the sensors resolution
	//  9 bit = 0.5 °C steps
	// 10 bit = 0.25 °C steps
	// etc.
	(void) config;
	
	int32_t convertedTemperature = 625L * temp;
	
	// round to centi-degree
	convertedTemperature = (convertedTemperature + 50) / 100;
	
	return (static_cast<int16_t>(convertedTemperature));
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
xpcc::DS18B20<OneWire>::selectDevice()
{
	// Reset the bus / Initialization
	if (!ow.touchReset()) {
		// no devices detected
		return false;
	}
	
	ow.writeByte(one_wire::MATCH_ROM);
	
	for (uint8_t i = 0; i < 8; ++i) {
		ow.writeByte(this->identifier[i]);
	}
	
	return true;
}
