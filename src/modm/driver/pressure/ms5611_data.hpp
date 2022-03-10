// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2021, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MS5611_DATA_HPP
#define MODM_MS5611_DATA_HPP

namespace modm
{

// Forward declaration of the driver class
template <typename SpiMaster, typename Cs>
class Ms5611;

namespace ms5611data
{

/**
 * @brief Holds the factory calibration data from the PROM.
 * Upon initialization the driver automatically reads the calibration values from the PROM
 */
/// @ingroup modm_driver_ms5611
struct modm_packed
Prom
{
    uint16_t
    calculateCrc()
    {
        uint16_t crcRead = data[7];
        uint16_t remainder = 0x00;
        data[7] = (0xFF00 & (data[7]));
        for (uint8_t count = 0; count < 16; count++)
        {
            if (count % 2 == 1)
            {
                remainder ^= static_cast<uint16_t>((data[count >> 1]) & 0x00FF);
            }
            else
            {
                remainder ^= static_cast<uint16_t>(data[count >> 1] >> 8);
            }

            for (uint8_t bit = 8; bit > 0; bit--)
            {
                if (remainder & (0x8000))
                {
                    remainder = (remainder << 1) ^ 0x3000;
                }
                else
                {
                    remainder = (remainder << 1);
                }
            }
        }
        remainder =  (0x000F & (remainder >> 12));
        data[7] = crcRead;
        return (remainder ^ 0x00);
    }

    /**
     * prom[0] Factory data and the setup
     * prom[1] Pressure sensitivity
     * prom[2] Pressure offset
     * prom[3] Temperature coefficient of pressure sensitivity
     * prom[4] Temperature coefficient of pressure offset
     * prom[5] Reference temperature
     * prom[6] Temperature coefficient of the temperature
     * prom[7] Serial code and CRC
     */
    uint16_t data[8];
};

/// @ingroup modm_driver_ms5611
class modm_packed
DataBase
{
    template <typename SpiMaster, typename Cs>
    friend class ::modm::Ms5611;

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
        uint8_t raw[6];

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

/// @ingroup modm_driver_ms5611
class modm_packed
Data : public DataBase
{
public:

    /**
     * @brief Get the calibrated pressure data with 0.01 mbar resolution
     * @attention No SPI transaction
     *
     * @return int32_t
     */
    int32_t inline
    getPressure();

    void inline
    getPressure(int32_t &pres) { pres = getPressure(); }

    void inline
    getPressure(float &pres) { pres = float(getPressure()) / 100.0f; }

    /**
     * @brief Get the calibrated temperature data with 0.01 degrees Centigrade resolution
     * @attention No SPI transaction
     *
     * @return int32_t
     */
    int32_t inline
    getTemperature();

    void inline
    getTemperature(int32_t &temp) { temp = getTemperature(); }

    void inline
    getTemperature(float &temp) { temp = float(getTemperature()) / 100.0f; }

protected:
    void inline
    calculateCalibratedValues();

private:
    int32_t calibratedPressure;
    int32_t calibratedTemperature;
};

} // ms5611data

} // modm namespace

#include "ms5611_data_impl.hpp"

#endif // MODM_MS5611_DATA_HPP