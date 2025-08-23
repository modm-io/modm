/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PAT9125EL_HPP
#	error  "Don't include this file directly, use 'pat9125el.hpp' instead!"
#endif

namespace modm
{

template<typename Transport, typename IntPin>
template<typename... TransportParams>
Pat9125el<Transport, IntPin>::Pat9125el(TransportParams&&... params)
	: Transport(std::forward<TransportParams>(params)...)
{
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::configure(uint8_t xResolution, uint8_t yResolution)
{
	success = writeRegister(Register::WriteProtect, uint8_t(WriteProtect::Disabled));
	if(!success) {
		return false;
	}

	success = writeRegister(Register::ResolutionX, xResolution);
	if(!success) {
		return false;
	}

	success = writeRegister(Register::ResolutionY, yResolution);
	if(!success) {
		return false;
	}

	return writeRegister(Register::WriteProtect, uint8_t(WriteProtect::Enabled));
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::ping()
{
	success = readRegister(Register::ProductId1, readBuffer[0]);
	if(!success) {
		return false;
	}

	return readBuffer[0] == ProductId1;
}

template<typename Transport, typename IntPin>
pat9125el::Motion2D
Pat9125el<Transport, IntPin>::getData() const
{
	return data;
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::readData()
{
	if constexpr(UseInterruptPin) {
		if(IntPin::read()) {
			return false;
		}
	}

	success = readRegister(Register::MotionStatus, status);
	if(!success) {
		return false;
	}

	if(MotionStatus_t{status} & MotionStatus::DataAvailable) {
		// read x and y low data registers
		success = readRegister(Register::DeltaXLow, &readBuffer[0], 2);
		if(!success) {
			return false;
		}

		// read x/y high data register
		success = readRegister(Register::DeltaXYHigh, &readBuffer[2], 1);
		if(!success) {
			return false;
		}

		updateData();
	}

	return true;
}

template<typename Transport, typename IntPin>
void
Pat9125el<Transport, IntPin>::updateData()
{
	const int16_t x = (readBuffer[0] | ((readBuffer[2] & 0b11110000) << 4));
	const int16_t y = (readBuffer[1] | ((readBuffer[2] & 0b00001111) << 8));

	// convert 12-bit 2's complement data to 16-bit signed int
	data.x = static_cast<int16_t>(x << 4) >> 4;
	data.y = static_cast<int16_t>(y << 4) >> 4;
	moved = true;
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::hasMoved() const
{
	return moved;
}

template<typename Transport, typename IntPin>
void
Pat9125el<Transport, IntPin>::resetMoved()
{
	moved = false;
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::writeRegister(Register reg, uint8_t data)
{
	return this->write(static_cast<uint8_t>(reg), data);
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::readRegister(Register reg, uint8_t& data)
{
	return this->read(static_cast<uint8_t>(reg), data);
}

template<typename Transport, typename IntPin>
bool
Pat9125el<Transport, IntPin>::readRegister(Register reg, uint8_t* data, size_t size)
{
	return this->read(static_cast<uint8_t>(reg), data, size);
}

}
