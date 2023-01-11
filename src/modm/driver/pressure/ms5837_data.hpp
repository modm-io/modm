// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Vivien Henry
 * Based on the ms5837, courtesy of Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MS5837_DATA_HPP
#define MODM_MS5837_DATA_HPP

namespace modm
{

// Forward declaration of the driver class
template < typename I2cMaster >
class Ms5837;

namespace ms5837data
{

/**
 * @brief Holds the factory calibration data from the PROM.
 * Upon initialization the driver automatically reads the calibration values from the PROM
 */
/// @ingroup modm_driver_ms5837
struct modm_packed
Prom
{
	uint8_t
	calculateCrc()
	{
		uint32_t n_rem = 0; // crc remainder
		data[0] = ((data[0]) & 0x0FFF); // CRC byte is replaced by 0
		data[7] = 0; // Subsidiary value, set to 0
		for (int cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
		{ // choose LSB or MSB
			if (cnt % 2==1) n_rem ^= uint16_t((data[cnt>>1]) & 0x00FF);
			else n_rem ^= uint16_t(data[cnt>>1] >> 8);

			for (uint8_t n_bit = 8; n_bit > 0; n_bit--)
			{
				if (n_rem & (0x8000)) n_rem = (n_rem << 1) ^ 0x3000;
				else n_rem = (n_rem << 1);
			}
		}
		n_rem = ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
		return n_rem;
	}

	/**
	 * prom[0] Factory data and CRC
	 * prom[1] C1 / Pressure sensitivity
	 * prom[2] C2 / Pressure offset
	 * prom[3] C3 / Temperature coefficient of pressure sensitivity
	 * prom[4] C4 / Temperature coefficient of pressure offset
	 * prom[5] C5 / Reference temperature
	 * prom[6] C6 / Temperature coefficient of the temperature
	 * prom[7] Subsidiary value (not read, used internally for calculation)
	 */
	uint16_t data[8]{};
};

/// @ingroup modm_driver_ms5837
class modm_packed
DataBase
{
	template < typename I2cMaster >
	friend class ::modm::Ms5837;

	public:
		inline
		DataBase() : meta(0) {}

		inline Prom&
		getProm() { return prom; }

	public:
		/// Notify the data class about changed buffer of raw pressure data (D1).
		void rawPressureTouched() { meta &= ~PRESSURE_CALCULATED; }

		/// Notify the data class about changed buffer of raw temperature data (D2).
		void rawTemperatureTouched() { meta &= ~TEMPERATURE_CALCULATED; }

	protected:
		Prom prom;

	protected:
		// The raw data read from the ADC register
		// 0 .. 2 pressure data (D1)
		// 3 .. 5 temperature data (D2)
		uint8_t raw[6]{};

		// Bit 1: Temperature calculated
		// Bit 0: Pressure calculated
		uint8_t meta = 0;

		enum
		{
			/// Remember if the raw data has been converted to pressure
			PRESSURE_CALCULATED = modm::Bit0,
			/// Remember if the raw data has been converted to temperature
			TEMPERATURE_CALCULATED = modm::Bit1,
		};
};

/// @ingroup modm_driver_ms5837
class modm_packed
Data : public DataBase
{
public:

	/**
	 * @brief Get the calibrated pressure data with 0.25 mbar resolution (in tenth of mbar (10^-4 bar))
	 * @attention No I2C transaction
	 *
	 * @return int32_t
	 */
	int32_t inline
	getPressure();

	void inline
	getPressure(int32_t &pres) { pres = getPressure(); }

	/**
	 * @brief Get the calibrated pressure data with 0.25 mbar resolution (in mbar)
	 * @attention No I2C transaction
	 * @return void
	 */
	void inline
	getPressure(float &pres) { pres = float(getPressure()) / 10.0f; }

	/**
	 * @brief Get the calibrated temperature data with 0.01 degrees Centigrade resolution (2501 = 25.01°C)
	 * @attention No I2C transaction
	 *
	 * @return int32_t
	 */
	int32_t inline
	getTemperature();

	void inline
	getTemperature(int32_t &temp) { temp = getTemperature(); }

	/**
	 * @brief Get the calibrated temperature data with 0.01 degrees Centigrade resolution, in °C
	 * @attention No I2C transaction
	 *
	 * @return void
	 */
	void inline
	getTemperature(float &temp) { temp = float(getTemperature()) / 100.0f; }


	inline uint32_t getRawC1() { return uint32_t(raw[0] << 16 | raw[1] << 8 | raw[2]); }
	inline uint32_t getRawC2() { return uint32_t(raw[3] << 16 | raw[4] << 8 | raw[5]); }

protected:
	void inline
	calculateCalibratedValues();

private:
	int32_t calibratedPressure;
	int32_t calibratedTemperature;
};

} // ms5837data namespace

} // modm namespace

#include "ms5837_data_impl.hpp"

#endif // MODM_MS5837_DATA_HPP
