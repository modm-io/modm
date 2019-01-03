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
#define MODM_PAT9125EL_HPP

#include <array>
#include <type_traits>
#include <modm/driver/motion/pat9125el_transport.hpp>
#include <modm/math/geometry/vector2.hpp>

namespace modm
{

/// @ingroup modm_driver_pat9125el
struct pat9125el
{
	enum class Register : uint8_t
	{
		ProductId1 		= 0x00,
		ProductId2 		= 0x01,
		MotionStatus	= 0x02,
		DeltaXLow		= 0x03,
		DeltaYLow		= 0x04,
		OperationMode	= 0x05,
		Configuration	= 0x06,
		WriteProtect	= 0x09,
		Sleep1			= 0x0A,
		Sleep2			= 0x0B,
		ResolutionX		= 0x0D,
		ResolutionY		= 0x0E,
		DeltaXYHigh		= 0x12,
		ShutterTime		= 0x14,
		AvgBrightness	= 0x17,
		Orientation		= 0x19
	};

	enum class MotionStatus : uint8_t
	{
		DataAvailable = (1 << 7)
	};
	MODM_FLAGS8(MotionStatus);

	enum class WriteProtect : uint8_t
	{
		Enabled = 0,
		Disabled = 0x5A
	};
	MODM_FLAGS8(WriteProtect);

	static constexpr uint8_t ProductId1 = 0x31;

	using Motion2D = modm::Vector<int16_t, 2>;
};

/**
 * Driver for PAT9125EL motion sensor. Currently only the I2C version
 * is supported.
 *
 * @tparam Transport Either I2C or SPi transport layer
 * @see Pat9125elI2cTransport
 *
 * @tparam IntPin Optional interrupt pin. If pin is not connected, set to void.
 *
 * @todo implement SPI transport layer
 * @todo add sensor sleep modes
 *
 * @author Christopher Durand
 * @ingroup modm_driver_pat9125el
 */
template<typename Transport, typename IntPin = void>
class Pat9125el : public pat9125el, public Transport
{
public:
	static constexpr bool UseInterruptPin = !std::is_void_v<IntPin>;

	/**
	 * @tparam TransportArgs Arguments forwarded to transport layer
	 * @see Pat9125elI2cTransport
	 */
	template<typename... TransportArgs>
	Pat9125el(TransportArgs&&... args);

	/**
	 * Configure sensor resolution
	 *
	 * @param xResolution x resolution in unit of 5 counts per inch
	 * @param yResolution y resolution in unit of 5 counts per inch
	 */
	modm::ResumableResult<bool>
	configure(uint8_t xResolution = 0x14, uint8_t yResolution = 0x14);

	/**
	 * Check if the device is available
	 */
	modm::ResumableResult<bool>
	ping();

	/**
	 * Returns the last measured movement data read from the device
	 */
	Motion2D
	getData() const;

	/**
	 * Read movement measurement from the device
	 */
	modm::ResumableResult<bool>
	readData();

	/**
	 * Check if non-zero movement data has been read form the device
	 */
	bool
	hasMoved() const;

	void
	resetMoved();

private:
	modm::ResumableResult<bool>
	writeRegister(Register reg, uint8_t data);

	modm::ResumableResult<bool>
	readRegister(Register reg, uint8_t& data);

	modm::ResumableResult<bool>
	readRegister(Register reg, uint8_t* data, size_t size);

	void
	updateData();

	Motion2D data = {};
	bool moved = false;
	uint8_t status = 0;
	bool success = true;
	std::array<uint8_t, 3> readBuffer = {};
};

}

#include "pat9125el_impl.hpp"

#endif // MODM_PAT9125EL_HPP
