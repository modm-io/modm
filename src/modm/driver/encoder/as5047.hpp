// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2024, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AS5047_HPP
#define MODM_AS5047_HPP

#include <array>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/processing/timer.hpp>
#include <numbers>

namespace modm
{

/// @cond
namespace detail
{
constexpr uint16_t
as5047_setMsbToEvenParity(const uint16_t num)
{
	uint16_t par = 0x7fff & num;
	par ^= par >> 8;
	par ^= par >> 4;
	par ^= par >> 2;
	par ^= par >> 1;
	return ((par & 1) << 15) | (0x7fff & num);
}
}  // namespace detail
/// @endcond

/// @ingroup modm_driver_as5047
struct as5047
{
	enum class Errorfl : uint16_t
	{
		Parerr = Bit2,
		Invcomm = Bit1,
		Frerr = Bit0,
	};
	MODM_FLAGS16(Errorfl)

	enum class Prog : uint16_t
	{
		Progver = Bit6,
		Progotp = Bit3,
		Otpref = Bit2,
		Progen = Bit0,
	};
	MODM_FLAGS16(Prog)

	enum class Diaagc : uint16_t
	{
		Magl = Bit11,
		Magh = Bit10,
		Cof = Bit9,
		Lf = Bit8,
	};
	MODM_FLAGS16(Diaagc)

	enum class Register : uint16_t
	{
		ReadNop = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x0000)),
		ReadErrfl = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x0001)),
		ReadProg = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x0003)),
		ReadDiaagc = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x3FFC)),
		ReadMag = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x3FFD)),
		ReadAngleunc = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x3FFE)),
		ReadAnglecom = detail::as5047_setMsbToEvenParity(((1 << 14) | 0x3FFF)),

	};

	struct modm_packed Data
	{
		/// @return
		constexpr float
		getAngleRad() const
		{
			const uint16_t angle = data & 0x3fff;
			return static_cast<float>(angle) / 16383.f * 2.f * std::numbers::pi_v<float>;
		}

		/// @return
		constexpr float
		getAngleDeg() const
		{
			const uint16_t angle = data & 0x3fff;
			return static_cast<float>(angle) / 16383.f * 360.f;
		}

		/// @return
		constexpr uint16_t
		getAngleRaw() const
		{
			const uint16_t angle = data & 0x3fff;
			return angle;
		}

		uint16_t data;
	};
};  // struct as5047

/**
 * @tparam SpiMaster
 * @tparam Cs
 *
 * @author  Henrik Hose
 * @ingroup modm_driver_as5047
 */
template<typename SpiMaster, typename Cs>
class As5047 : public as5047, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<5>
{
public:
	using Data = as5047::Data;

	/**
	 * @param data pointer to buffer of the internal data of type Data
	 */
	As5047(Data &data);

	/// Call this function once before using the device
	modm::ResumableResult<void>
	initialize();

	/// Read the raw data from the sensor
	modm::ResumableResult<void>
	readout();

	/// Get the data object for this sensor
	inline Data &
	getData()
	{
		return data;
	}

private:
	Data &data;
	uint8_t inBuffer[2];
	uint8_t outBuffer[2];
};

}  // namespace modm

#include "as5047_impl.hpp"

#endif  // MODM_AS5047_HPP