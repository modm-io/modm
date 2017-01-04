/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DS18B20_HPP
	#error	"Don't include this file directly, use 'ds18b20.hpp' instead!"
#endif

#include <cstring>		// for std::memcpy

template <typename OneWire>
xpcc::Ds18b20<OneWire>::Ds18b20(const uint8_t *rom)
{
	std::memcpy(this->identifier, rom, 8);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
xpcc::Ds18b20<OneWire>::isAvailable()
{
	return ow.verifyDevice(this->identifier);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
void
xpcc::Ds18b20<OneWire>::startConversion()
{
	selectDevice();
	
	ow.writeByte(CONVERT_T);
}

template <typename OneWire>
void
xpcc::Ds18b20<OneWire>::startConversions()
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
xpcc::Ds18b20<OneWire>::isConversionDone()
{
	return ow.readBit();
}

// ----------------------------------------------------------------------------
template <typename OneWire>
int16_t
xpcc::Ds18b20<OneWire>::readTemperature()
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
xpcc::Ds18b20<OneWire>::selectDevice()
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
