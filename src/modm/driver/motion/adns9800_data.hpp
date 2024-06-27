// coding: utf-8
/*
 * Copyright (c) 2024, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/math/geometry/vector2.hpp>
#include <modm/math/utils/bit_constants.hpp>
#include <modm/math/utils/endianness.hpp>
#include <span>

#include "adns9800.hpp"

/**
 * @brief   Control the amount of information to stream from the sensor
 *          by choosing one of three Data types with increasing details.
 * @author  Thomas Sommer
 * @ingroup modm_driver_adns9800
 */
namespace modm
{

/**
 * @brief			The smallest data packet contains:
 * 						- relative motion vector
 */
struct adns9800::Data
{
	using Span = std::span<uint8_t, 6>;

	const modm::Vector<Delta, 2> delta;

protected:
	Data(Span data)
		: delta{
      static_cast<Delta>(data[3] << 8 | data[2]),
			static_cast<Delta>(data[5] << 8 | data[4])
    }
	{}

	template<class, class>
	friend class Adns9800;
};

/**
 * @brief			This data packet contains:
 * 						- relative motion vector
 * 						- laser fault detection flags
 */
struct adns9800::Data_FailFlags : adns9800::Data
{
	const bool LaserFaultDetected : 1;
	const bool LaserPowerValid : 1;
	const bool isRunningSROMCode : 1;

protected:
	Data_FailFlags(Span data)
		: Data(data),
		  LaserFaultDetected(data[0] & Bit6),
		  LaserPowerValid(data[0] & Bit5),
		  isRunningSROMCode(data[1] & Bit6)
	{}

	template<class, class>
	friend class Adns9800;
};

/**
 * @brief			The biggest data packet contains:
 * 						- relative motion vector
 * 						- laser fault detection flags
 * 						- shutter metrics like exposure time or image mean
 */
struct adns9800::Data_FailFlags_Monitoring : adns9800::Data_FailFlags
{
	using Span = std::span<uint8_t, adns9800::Data_FailFlags::Span::extent + 8>;

	struct Metrics
	{
		/**
		 * Number of features visible by the sensor in the current frame. Range 0 to 169.
		 * Total number of features: 4 * surface_quality.
		 * Changes are expected when moving over a surface.
		 * Convergates to 0 if there is no surface below the sensor.
		 * surface_quality remains fairly high throughout the Z-height.
		 */
		const uint8_t surface_quality;
		// pixel_sum containes the average pixel value. Range 0 to 223.
		// It reports the upper byte of a 17-bit counter which sums all 900 pixels in the current
		// frame
		const uint8_t pixel_sum;
		// Minium and maximum Pixel value in current frame. Range: 0 to 127.
		const Pixel max_pixel;
		const Pixel min_pixel;
	} metrics;

	Pixel
	getMean() const
	{
		return metrics.pixel_sum << 9 / pixel_count;
	}

	struct Shutter
	{
		// exposure <= ShutterConfig::exposure_max!
		const Period exposure;
		// ShutterConfig::period_min <= period <= ShutterConfig::period_max!
		const Period period;
	} shutter;

	Duration
	getExposureTime() const
	{
		return Duration(shutter.exposure);
	};
	Duration
	getFrameTime() const
	{
		return Duration(shutter.period);
	};

protected:
	Data_FailFlags_Monitoring(Span data)
		: Data_FailFlags(data.subspan<0, Data_FailFlags::Span::extent>()),
		  metrics{
				// @optimize: construct the Data class in union with the buffer,
				// -> no more byte copies required:
			  surface_quality : data[6],
			  pixel_sum : data[7],
			  max_pixel : data[8],
			  min_pixel : data[9]
		  },
		  shutter{
			  exposure : static_cast<Period>(data[10] << 8 | data[11]),
			  period : static_cast<Period>(data[12] << 8 | data[13])
		  }
	{}

	template<class, class>
	friend class Adns9800;
};
}  // namespace modm