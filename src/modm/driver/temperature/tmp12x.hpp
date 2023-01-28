/*
 * Copyright (c) 2020, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TMP12x_HPP
#define MODM_TMP12x_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable/nested_resumable.hpp>
#include <array>
#include <ratio>

namespace modm
{

/**
 * TMP12x temperature data
 * Use aliases Tmp121Temperature, Tmp123Temperature, Tmp125Temperature
 * for the respective sensor.
 *
 * @ingroup modm_driver_tmp12x
 * @author	Christopher Durand
 *
 * @tparam numBits Number of bits of measured temperature
 * @tparam offsetBits Invalid LSB of raw sensor value
 * @tparam numerator
 * @tparam denominator
 */
template<
	uint8_t NumBits,
	uint8_t OffsetBits,
	uint8_t Numerator,
	uint8_t Denominator
>
class Tmp12xTemperature
{
public:
	using Fraction = std::ratio<Numerator, Denominator>;

	constexpr Tmp12xTemperature() = default;

	constexpr explicit Tmp12xTemperature(uint16_t sensorData)
	{
		static_assert((NumBits + OffsetBits) <= 16);

		// mask out invalid bits
		constexpr uint16_t mask = ((1U << NumBits) - 1u) << OffsetBits;
		const uint16_t rawTemp = sensorData & mask;
		// convert to int16_t 2's complement representation
		temperature = int16_t(rawTemp << (16 - OffsetBits - NumBits)) >> (16 - NumBits);
	}

	constexpr float
	getTemperatureFloat() const
	{
		return temperature * (Fraction::num / float(Fraction::den));
	}

	constexpr int16_t
	getTemperatureInteger() const
	{
		return temperature * Fraction::num / Fraction::den;
	}

	constexpr int16_t
	getTemperatureFractional() const
	{
		return temperature;
	}

private:
	int16_t temperature = 0;
};

/// @ingroup modm_driver_tmp12x
using Tmp121Temperature = Tmp12xTemperature<13, 3, 1, 16>; // 12+1 bit, 1/16th °C resolution

/// @ingroup modm_driver_tmp12x
using Tmp123Temperature = Tmp121Temperature;

/// @ingroup modm_driver_tmp12x
using Tmp125Temperature = Tmp12xTemperature<10, 5, 1, 4>; // 10 bit, 1/4 °C resolution

/**
 * TMP12x temperature sensor driver
 * Use aliases Tmp121, Tmp123, Tmp125 for convenience.
 *
 * @ingroup modm_driver_tmp12x
 * @author	Christopher Durand
 *
 * @tparam SpiMaster SPI master interface
 * @tparam Cs Chip select output
 * @tparam TemperatureT Sensor temperature type
 */
template<typename SpiMaster, typename Cs, typename TemperatureT>
class Tmp12x : public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<1>
{
public:
	using Temperature = TemperatureT;

	Tmp12x()
	{
		this->attachConfigurationHandler([]() {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
			SpiMaster::setDataSize(SpiMaster::DataSize::Bit8);
		});
	}

	void
	initialize()
	{
		Cs::setOutput(true);
	}

	modm::ResumableResult<Temperature>
	read()
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();
		RF_CALL(SpiMaster::transfer(nullptr, buffer_.data(), 2));

		if (this->releaseMaster()) {
			Cs::set();
		}

		RF_END_RETURN(Temperature(buffer_[1] | (buffer_[0] << 8)));
	}
private:
	std::array<uint8_t, 2> buffer_{};
};

/**
 * TMP121 temperature sensor driver
 *
 * @tparam SpiMaster SPI master interface
 * @tparam Cs Chip select output
 */
template<typename SpiMaster, typename Cs>
using Tmp121 = Tmp12x<SpiMaster, Cs, Tmp121Temperature>;

/**
 * TMP123 temperature sensor driver
 *
 * @tparam SpiMaster SPI master interface
 * @tparam Cs Chip select output
 */
template<typename SpiMaster, typename Cs>
using Tmp123 = Tmp12x<SpiMaster, Cs, Tmp123Temperature>;

/**
 * TMP125 temperature sensor driver
 *
 * @tparam SpiMaster SPI master interface
 * @tparam Cs Chip select output
 */
template<typename SpiMaster, typename Cs>
using Tmp125 = Tmp12x<SpiMaster, Cs, Tmp125Temperature>;

} // namespace modm

#endif // MODM_TMP12x_HPP
