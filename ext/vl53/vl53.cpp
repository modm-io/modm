/*
 * Copyright (c) 2025, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "vl53_transport.hpp"

extern "C" {
#include "vl53_platform.h"

uint8_t
VL53_RdByte(VL53_Platform *p_platform, uint16_t RegisterAddress, uint8_t *p_value)
{
	auto *iface = static_cast<modm::Vl53TransportBase *>(p_platform->transport);
	return iface->readByte(RegisterAddress, p_value);
}

uint8_t
VL53_WrByte(VL53_Platform *p_platform, uint16_t RegisterAddress, uint8_t value)
{
	auto *iface = static_cast<modm::Vl53TransportBase *>(p_platform->transport);
	return iface->writeByte(RegisterAddress, value);
}

uint8_t
VL53_WrMulti(VL53_Platform *p_platform, uint16_t RegisterAddress, uint8_t *p_values, uint32_t size)
{
	auto *iface = static_cast<modm::Vl53TransportBase *>(p_platform->transport);
	return iface->writeMulti(RegisterAddress, p_values, size);
}

uint8_t
VL53_RdMulti(VL53_Platform *p_platform, uint16_t RegisterAddress, uint8_t *p_values, uint32_t size)
{
	auto *iface = static_cast<modm::Vl53TransportBase *>(p_platform->transport);
	return iface->readMulti(RegisterAddress, p_values, size);
}

void
VL53_SwapBuffer(uint8_t *buffer, uint16_t size)
{
	uint32_t i, tmp;

	for (i = 0; i < size; i = i + 4)
	{
		tmp = (buffer[i] << 24) | (buffer[i + 1] << 16) | (buffer[i + 2] << 8) | (buffer[i + 3]);
		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t
VL53_WaitMs(VL53_Platform*, uint32_t TimeMs)
{
	modm::this_fiber::sleep_for(std::chrono::milliseconds(TimeMs));
	return 0;
}

uint8_t
VL53_Reset_Sensor(VL53_Platform *p_platform)
{
	auto *iface = static_cast<modm::Vl53TransportBase *>(p_platform->transport);
	return iface->resetSensor();
}
}
